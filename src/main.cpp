#include "bs.hpp"

#include <chrono>

int main(int argc, const char* argv[]) try 
{
    std::unique_ptr<bs::IDeviceSearcher> searcher = bs::createDeviceSearcher();

    cl::Device device = searcher->getDevice(1, 0);

    #ifdef DEBUG
    std::cout << "Selected device: \n";
    bs::printDeviceInfo(device);
    #endif

    std::vector<int> sequence;

    if (argc > 1)
    {
        sequence = bs::input_fstream<int>(argv[1]);
    }
    else
    {
        sequence = bs::input_stdin<int>();
    }

    size_t sequence_size = sequence.size();

    if ((sequence_size > 0) && !((sequence_size & (sequence_size - 1)) == 0))
    {
        size_t new_size = 1;
        while (new_size < sequence_size) {
            new_size <<= 1;
        }

        sequence.resize(new_size);
    }


    std::string kernelSource = bs::readKernel("src/bitonicSort_gkernel.cl") + 
                               bs::readKernel("src/bitonicSort_lkernel.cl");
    #ifdef DEBUG
    std::cout << kernelSource << '\n';
    #endif


    #ifdef COMPARE
    std::vector<int> sequence2 = sequence;
    #endif

    // auto start1 = std::chrono::high_resolution_clock::now();
    // bs::bitonicSort(sequence, device, kernelSource);
    // auto end1 = std::chrono::high_resolution_clock::now();

    #if !defined(COMPARE) && !defined(DEBUG)
    bs::bitonicSort_modernized(sequence, device, kernelSource);

    for (float v : sequence) std::cout << v << " ";
        std::cout << std::endl;
    #endif

    #ifdef COMPARE
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
    #endif
}
catch (const std::exception& e)
{
    std::cout << "Error: " << e.what() << std::endl;
}
catch (...)
{
    std::cout << "Unknown problems occurred\n";
}

