#pragma once

#include <iostream>
#include <vector>  
#include <exception> 
#include <memory>
#include <optional>
#include <fstream>
#include <sstream>
#include <algorithm>

#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 300
#define CL_HPP_MINIMUM_OPENCL_VERSION 300

#include "opencl.hpp"

namespace bs {

void printDeviceInfo(const cl::Device& device);

class IDeviceSearcher
{
public:
    virtual cl::Device 
    getDevice(size_t platformIdx, size_t deviceIdx) const = 0;

    virtual cl::Device
    getFirstSuitableDevice() const = 0;
    
    virtual size_t
    getPlatformsCount() const = 0;
    
    virtual size_t
    getDevicesCount(size_t platformIdx) const = 0;
    
    virtual void
    showAllDevicesInfo() const = 0;
    
    virtual ~IDeviceSearcher() = default;
};

class DeviceSearcher final : public IDeviceSearcher
{
    struct PlatformData
    {
        cl::Platform platform;
        std::vector<cl::Device> devices;
    };

    std::vector<PlatformData> platformsData;

public:
    DeviceSearcher() 
    {
        try
        {
            std::vector<cl::Platform> platforms;
            cl::Platform::get(&platforms);
            
            if (platforms.empty())
                throw std::runtime_error("No OpenCL platforms found");
            
            platformsData.reserve(platforms.size());
            
            for (const auto& platform : platforms)
            {
                PlatformData platformData;
                platformData.platform = platform;
                
                std::vector<cl::Device> devices;
                platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
                platformData.devices = std::move(devices);
                
                platformsData.push_back(std::move(platformData));
            }
        }
        catch (const cl::Error& e)
        {
            std::cerr << "OpenCL error during initialization: " 
                      << e.what() << " (" << e.err() << ")" << std::endl;
            throw std::runtime_error("Failed to initialize DeviceSearcher");
        }
    }

    virtual cl::Device
    getDevice(size_t platformIdx, size_t deviceIdx) const override
    {
        if (platformIdx >= platformsData.size())
        {
            throw std::out_of_range(
                "Platform index "                 +
                std::to_string(platformIdx)       + 
                " is out of range. Max index is " + 
                std::to_string(platformsData.size() - 1)
            );
        }

        if (deviceIdx >= platformsData[platformIdx].devices.size())
        {
            throw std::out_of_range(
                "Device index "                  + 
                std::to_string(deviceIdx)        + 
                " is out of range for platform " + 
                std::to_string(platformIdx)      + 
                ". Max index is "                + 
                std::to_string(platformsData[platformIdx].devices.size() - 1)
            );
        }

        return platformsData[platformIdx].devices[deviceIdx];
    }

    virtual cl::Device
    getFirstSuitableDevice() const override
    {
        auto findDevice = [this](cl_device_type preferredType) -> std::optional<cl::Device> 
        {
            for (const auto& platformData : platformsData)
            {
                for (const auto& device : platformData.devices)
                {
                    cl_device_type type;
                    device.getInfo(CL_DEVICE_TYPE, &type);
                    
                    if (type == preferredType)
                        return device;
                }
            }
            return std::nullopt;
        };
        
        if (auto device_optionalWrap = findDevice(CL_DEVICE_TYPE_GPU))
            return *device_optionalWrap;
        
        if (auto device_optionalWrap = findDevice(CL_DEVICE_TYPE_CPU))
            return *device_optionalWrap;
        
        for (const auto& platformData : platformsData)
        {
            if (!platformData.devices.empty())
                return platformData.devices[0];
        }
        
        throw std::runtime_error("No OpenCL devices available");
    }
    
    virtual size_t
    getPlatformsCount() const override
    {
        return platformsData.size();
    }
    
    virtual size_t
    getDevicesCount(size_t platformIdx) const override
    {
        if (platformIdx >= platformsData.size())
        {
            throw std::out_of_range(
                "Platform index " + std::to_string(platformIdx) + 
                " is out of range"
            );
        }
        
        return platformsData[platformIdx].devices.size();
    }
    
    virtual void
    showAllDevicesInfo() const override
    {
        std::cout << "Found " << platformsData.size() << " OpenCL platform(s)\n\n";
        
        for (size_t i = 0; i < platformsData.size(); ++i)
        {
            std::string platformName;
            platformsData[i].platform.getInfo(CL_PLATFORM_NAME, &platformName);
            
            std::cout << "=== Platform " << i << ": " << platformName 
                      << " ===\n";
            std::cout << "Devices: " << platformsData[i].devices.size() << "\n\n";
            
            for (size_t j = 0; j < platformsData[i].devices.size(); ++j)
            {
                std::cout << "  Device " << j << ":\n";
                printDeviceInfo(platformsData[i].devices[j]);
                std::cout << std::endl;
            }
            
            std::cout << std::string(50, '-') << "\n\n";
        }
    }
};

void printDeviceInfo(const cl::Device& device)
{
    try
    {
        std::string deviceName;
        std::string deviceVendor;
        cl_device_type deviceType;
        cl_uint maxComputeUnits;
        cl_ulong globalMemSize;
        cl_ulong localMemSize;
        cl_ulong cacheSize;
        size_t maxWorkGroupSize;
        
        device.getInfo(CL_DEVICE_NAME, &deviceName);
        device.getInfo(CL_DEVICE_VENDOR, &deviceVendor);
        device.getInfo(CL_DEVICE_TYPE, &deviceType);
        device.getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &maxComputeUnits);
        device.getInfo(CL_DEVICE_GLOBAL_MEM_SIZE, &globalMemSize);
        device.getInfo(CL_DEVICE_LOCAL_MEM_SIZE, &localMemSize);
        device.getInfo(CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, &cacheSize);
        device.getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE, &maxWorkGroupSize);
        
        // std::cout << "    Platform index:      " << platformIdx << "\n";
        std::cout << "Name:                " << deviceName << "\n";
        std::cout << "Vendor:              " << deviceVendor << "\n";
        std::cout << "Type:                ";
        
        if (deviceType & CL_DEVICE_TYPE_CPU) std::cout << "CPU ";
        if (deviceType & CL_DEVICE_TYPE_GPU) std::cout << "GPU ";
        if (deviceType & CL_DEVICE_TYPE_ACCELERATOR) std::cout << "Accelerator ";
        if (deviceType & CL_DEVICE_TYPE_DEFAULT) std::cout << "Default ";
        
        std::cout << "\n";
        std::cout << "Max compute units:   " << maxComputeUnits << "\n";
        std::cout << "Global memory:       " 
                    << (globalMemSize / (1024 * 1024)) << " MB\n";
        std::cout << "Local memory:        " 
                    << (localMemSize / 1024) << " KB\n";
        std::cout << "Cache size:          " 
                    << (cacheSize / 1024) << " KB\n";
        std::cout << "Max work-group size: " << maxWorkGroupSize << "\n";
    }
    catch (const cl::Error& e)
    {
        std::cerr << "    Error getting device info: " 
                    << e.what() << " (" << e.err() << ")\n";
    }
}

std::unique_ptr<IDeviceSearcher> createDeviceSearcher()
{
    return std::make_unique<DeviceSearcher>();
}


// Bitonic sort: 5.63319 s
// std::sort: 5.8923 s

template <typename T>
void bitonicSort(std::vector<T>& sequence, const cl::Device& device, const std::string& kernelSource)
{
    size_t n = sequence.size();

    cl::Context context(device);
    cl::CommandQueue queue(context, device);

    cl::Buffer buffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 
                      sizeof(int) * sequence.size(), sequence.data());

    cl::Program program(context, kernelSource);

    try
    {
        program.build(device);
    }
    catch (...)
    {
        std::string buildLog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
        std::cerr << "Build failed: " << buildLog << std::endl;
        throw;
    }

    cl::Kernel gkernel(program, "bitonicStep_gkernel");

    for (int stage = 2; stage <= n; stage *= 2)
    {
        for (int subStage = stage / 2; subStage > 0; subStage /= 2)
        {   
            gkernel.setArg(0, buffer);
            gkernel.setArg(1, stage);
            gkernel.setArg(2, subStage);
            gkernel.setArg(3, 1);
            
            queue.enqueueNDRangeKernel(
                gkernel, 
                cl::NullRange,
                cl::NDRange(n),
                cl::NullRange
            );
            
            queue.finish();
        }
    }

    queue.enqueueReadBuffer(buffer, CL_TRUE, 0, sizeof(int) * sequence.size(), sequence.data());
}

template <typename T>
void bitonicSort_modernized(std::vector<T>& sequence, const cl::Device& device, const std::string& kernelSource)
{
    size_t n = sequence.size();

    cl::Context context(device);
    cl::CommandQueue queue(context, device);

    cl::Buffer buffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 
                      sizeof(int) * sequence.size(), sequence.data());

    cl::Program program(context, kernelSource);

    try
    {
        program.build(device);
    }
    catch (...)
    {
        std::string buildLog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
        std::cerr << "Build failed: " << buildLog << std::endl;
        throw;
    }

    cl::Kernel gkernel(program, "bitonicStep_gkernel");
    cl::Kernel lkernel(program, "bitonicStep_lkernel");

    size_t localSize_max = device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();

    for (int stage = 2; stage <= n; stage *= 2)
    {
        if (stage <= localSize_max)
        {
            size_t localSize = stage;
            
            lkernel.setArg(0, buffer);
            lkernel.setArg(1, stage);
            lkernel.setArg(2, cl::Local(sizeof(int) * localSize));
            lkernel.setArg(3, (int)n);
            
            queue.enqueueNDRangeKernel(
                lkernel, 
                cl::NullRange,
                cl::NDRange(n),
                cl::NDRange(localSize)
            );
        }
        else
        {   
            for (int subStage = stage / 2; subStage > 0; subStage /= 2)
            {   
                gkernel.setArg(0, buffer);
                gkernel.setArg(1, stage);
                gkernel.setArg(2, subStage);
                gkernel.setArg(3, 1);
                
                queue.enqueueNDRangeKernel(
                    gkernel, 
                    cl::NullRange,
                    cl::NDRange(n),
                    cl::NullRange
                );
                
                queue.finish();
            }
        }
        
        queue.finish();
    }

    queue.enqueueReadBuffer(buffer, CL_TRUE, 0, sizeof(int) * sequence.size(), sequence.data());
}

void stdSort(std::vector<int>& sequence)
{
    std::sort(sequence.begin(), sequence.end());
}

std::string readKernel(std::string_view fileWithKernel) 
{
    std::ifstream kernelFile(fileWithKernel.data());

    if (!kernelFile.is_open())
    {
        throw std::runtime_error("Can't open kernel file");
    }

    std::stringstream buffer;
    buffer << kernelFile.rdbuf();

    std::string kernel = buffer.str();

    kernelFile.close();

    return kernel;
}


template <typename T>
std::vector<T> input_stdin()
{
    long long count = 0;

    if (!(std::cin >> count)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw std::runtime_error("Invalid input. Expected a number.");
    }

    if (count < 0) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw std::runtime_error("Number must be non-negative.");
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    size_t n = static_cast<size_t>(count);
    std::vector<T> result;
    result.reserve(n);
    
    T value;
    size_t items_read = 0;
    
    while (items_read < n && std::cin >> value) {
        result.push_back(value);
        items_read++;
    }
    
    if (items_read != n) {
        throw std::runtime_error("Failed to read " + std::to_string(n) + 
                                 " numbers. Only " + std::to_string(items_read) + 
                                 " were provided.");
    }
    
    return result;
}

template <typename T>
std::vector<T> input_fstream(std::string_view fileName)
{
    std::ifstream inputFile(fileName.data());

    if (!inputFile.is_open()) {
        throw std::runtime_error("Failed to open file: " + std::string(fileName));
    }

    long long count = 0;
    
    if (!(inputFile >> count)) {
        throw std::runtime_error("Invalid file format. Expected a number at the beginning.");
    }

    if (count < 0) {
        throw std::runtime_error("Number of elements must be non-negative");
    }
    
    size_t n = static_cast<size_t>(count);
    std::vector<T> result;
    result.reserve(n);
    
    T value;
    size_t items_read = 0;
    
    while (items_read < n && inputFile >> value) {
        result.push_back(value);
        items_read++;
    }
    
    if (items_read != n) {
        throw std::runtime_error("File contains fewer numbers than specified. Expected: " + 
                                 std::to_string(n) + ", got: " + std::to_string(items_read));
    }

    return result;
}

}; // namespace bs