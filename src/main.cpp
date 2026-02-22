#include "bs.hpp"

#include <chrono>
#include <cxxopts.hpp>


void prepareSequenceForBS(std::vector<int>& sequence);
void showBitonicSort(std::vector<int>& sequence, const cl::Device& device, const std::string& kernelSource, const size_t initial_size);
void compare(std::vector<int>& sequence, const cl::Device& device, const std::string& kernelSource);

int main(int argc, const char* argv[]) try 
{
    cxxopts::Options options("biton", "Bitonic sort using OpenCL");
    options.add_options()
        ("f,file", "Input file with numbers to sort", cxxopts::value<std::string>())
        ("c,compare", "Compare with std::sort")
        ("h,help", "Print usage")
        ("dev", "Show selected OpenCL device")
        ("shdevs", "Show all available OpenCL devices")
        ("s,select", "Select device by platform and device index (format: <platformIdx>:<deviceIdx>)", cxxopts::value<std::string>()->default_value("auto"));


    auto result = options.parse(argc, argv);
    if (result.count("help"))
    {
      std::cout << options.help() << std::endl;
      exit(0);
    }

    std::vector<int> sequence;


    if (result.count("file"))
    {
        sequence = bs::input_fstream<int>(result["file"].as<std::string>());
    }


    if (result.count("shdevs"))
    {
        auto searcher = bs::createDeviceSearcher();
        searcher->showAllDevicesInfo();
        exit(0);
    }

    auto searcher = bs::createDeviceSearcher();
    cl::Device device;

    if (result["select"].as<std::string>() != "auto")
    {
        auto selectStr = result["select"].as<std::string>();

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

    if (result.count("dev"))
    {
        std::cout << "Selected device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
    }


    if (not result.count("file"))
    {
        sequence = bs::input_stdin<int>();
    }

    size_t initial_size = sequence.size();

    std::string kernelSource = bs::readKernel("src/bitonicSort_gkernel.cl") + 
                               bs::readKernel("src/bitonicSort_lkernel.cl");
    
    if (!sequence.empty())
    {
        prepareSequenceForBS(sequence);
        
        if(result.count("compare"))
        {
            std::vector<int> duplicate = sequence;
            compare(duplicate, device, kernelSource);
            exit(0);
        }

        showBitonicSort(sequence, device, kernelSource, initial_size);
    }
    
}
catch (const std::exception& e)
{
    std::cout << "Error: " << e.what() << std::endl;
}
catch (...)
{
    std::cout << "Unknown problems occurred\n";
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
