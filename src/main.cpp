import commandLineHandler;

#include "bs.hpp"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>
#include <chrono>

void prepareSequenceForBS(std::vector<int>& sequence);
void showBitonicSort(std::vector<int>& sequence, const cl::Device& device, const std::string& kernelSource, const size_t initial_size);
void compare(std::vector<int>& sequence, const cl::Device& device, const std::string& kernelSource);

int main(int argc, const char* argv[]) try
{
    auto options = clh::parseCommandLine(argc, argv);

    std::vector<int> sequence;

    if (!options.inputFile.empty())
    {
        sequence = bs::input_fstream<int>(options.inputFile);
    }

    if (options.showAllDevices)
    {
        auto searcher = bs::createDeviceSearcher();
        searcher->showAllDevicesInfo();
        return 0;
    }

    auto searcher = bs::createDeviceSearcher();
    cl::Device device;

    if (options.selectDevice != "auto")
    {
        auto selectStr = options.selectDevice;

        auto colonPos = selectStr.find(':');

        if (colonPos == std::string::npos)
        {
            throw std::runtime_error("Invalid format for --select. Expected <platformIdx>:<deviceIdx>");
        }
        size_t platformIdx = std::stoul(selectStr.substr(0, colonPos));
        size_t deviceIdx = std::stoul(selectStr.substr(colonPos + 1));
        device = searcher->getDevice(platformIdx, deviceIdx);
    }
    else
    {
        device = searcher->getFirstSuitableDevice();
    }

    if (options.showDevice)
    {
        std::cout << "Selected device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
    }

    if (options.inputFile.empty())
    {
        sequence = bs::input_stdin<int>();
    }

    size_t initial_size = sequence.size();

    std::string kernelSource = bs::readKernel("src/bitonicSort_gkernel.cl") +
                               bs::readKernel("src/bitonicSort_lkernel.cl");

    if (!sequence.empty())
    {
        prepareSequenceForBS(sequence);

        if (options.compare)
        {
            std::vector<int> duplicate = sequence;
            compare(duplicate, device, kernelSource);
            return 0;
        }

        showBitonicSort(sequence, device, kernelSource, initial_size);
    }

    return 0;
}
catch (const std::exception& e)
{
    std::cout << "Error: " << e.what() << std::endl;
    return 1;
}
catch (...)
{
    std::cout << "Unknown problems occurred\n";
    return 1;
}

void prepareSequenceForBS(std::vector<int>& sequence)
{
    size_t sequence_size = sequence.size();

    if ((sequence_size > 0) && !((sequence_size & (sequence_size - 1)) == 0))
    {
        size_t new_size = 1;
        while (new_size < sequence_size) {
            new_size <<= 1;
        }

        sequence.resize(new_size, std::numeric_limits<int>::max());
    }
}

void showBitonicSort(std::vector<int>& sequence,
                     const cl::Device& device,
                     const std::string& kernelSource,
                     const size_t initial_size)
{
    bs::bitonicSort_modernized(sequence, device, kernelSource);

    for (size_t i = 0; i < initial_size; i++) std::cout << sequence[i] << " ";

    std::cout << '\n';
}

void compare(std::vector<int>& sequence, const cl::Device& device, const std::string& kernelSource)
{
    std::vector<int> sequence2 = sequence;

    auto start1 = std::chrono::high_resolution_clock::now();
    bs::bitonicSort_modernized(sequence, device, kernelSource);
    auto end1 = std::chrono::high_resolution_clock::now();

    auto start2 = std::chrono::high_resolution_clock::now();
    bs::stdSort(sequence2);
    auto end2 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> diff1 = end1 - start1;
    std::chrono::duration<double> diff2 = end2 - start2;

    std::cout << "Bitonic sort: " << diff1.count() << " s\n";
    std::cout << "std::sort: " << diff2.count() << " s\n";
}
