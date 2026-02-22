#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>
#include <random>
#include <algorithm>
#include <sstream>
#include <fstream>

#include "bs.hpp"

using namespace bs;
using ::testing::HasSubstr;

TEST(DeviceSearcher, CreatesWithoutThrowIfAtLeastOnePlatformExists)
{
    EXPECT_NO_THROW({
        auto searcher = createDeviceSearcher();
        EXPECT_NE(searcher->getPlatformsCount(), 0u) << "No OpenCL platforms found";
    });
}

TEST(DeviceSearcher, GetDevice_ThrowsOnInvalidPlatformIndex)
{
    auto searcher = createDeviceSearcher();
    size_t platCount = searcher->getPlatformsCount();

    EXPECT_THROW(searcher->getDevice(platCount, 0), std::out_of_range);
    EXPECT_THROW(searcher->getDevice(999999, 0), std::out_of_range);
}

TEST(DeviceSearcher, GetDevice_ThrowsOnInvalidDeviceIndex)
{
    auto searcher = createDeviceSearcher();
    if (searcher->getPlatformsCount() == 0) {
        GTEST_SKIP() << "Нет платформ OpenCL";
    }

    size_t p = 0;
    size_t devCount = searcher->getDevicesCount(p);

    EXPECT_THROW(searcher->getDevice(p, devCount), std::out_of_range);
    EXPECT_THROW(searcher->getDevice(p, 999999), std::out_of_range);
}

TEST(DeviceSearcher, GetFirstSuitableDevice_PreferGpu)
{
    auto searcher = createDeviceSearcher();

    cl::Device dev;
    EXPECT_NO_THROW(dev = searcher->getFirstSuitableDevice());

    cl_device_type type = 0;
    dev.getInfo(CL_DEVICE_TYPE, &type);

    // Если есть GPU то должен вернуть GPU
    bool hasGpu = false;
    for (size_t p = 0; p < searcher->getPlatformsCount(); ++p) {
        for (size_t d = 0; d < searcher->getDevicesCount(p); ++d) {
            cl::Device dd = searcher->getDevice(p, d);
            cl_device_type t = 0;
            dd.getInfo(CL_DEVICE_TYPE, &t);
            if (t & CL_DEVICE_TYPE_GPU) hasGpu = true;
        }
    }

    if (hasGpu) {
        EXPECT_TRUE(type & CL_DEVICE_TYPE_GPU);
    }
}

TEST(InputStdin, ReadsCorrectCountAndValues)
{
    std::istringstream fakeInput(
        "5\n"
        "10 -3 7 42 0\n"
    );

    std::streambuf* old = std::cin.rdbuf(fakeInput.rdbuf());

    auto vec = input_stdin<int>();

    std::cin.rdbuf(old);

    ASSERT_EQ(vec.size(), 5u);
    EXPECT_THAT(vec, ::testing::ElementsAre(10, -3, 7, 42, 0));
}

TEST(InputStdin, ThrowsWhenNotEnoughNumbers)
{
    std::istringstream fakeInput("3\n1 2");  // только два числа

    std::streambuf* old = std::cin.rdbuf(fakeInput.rdbuf());

    EXPECT_THROW(input_stdin<int>(), std::runtime_error);

    std::cin.rdbuf(old);
}

TEST(InputFstream, CanReadFromTemporaryFile)
{
    const char* fname = "test_input_tmp.txt";
    {
        std::ofstream f(fname);
        f << "4\n-100 500 0 777\n";
    }

    auto vec = input_fstream<int>(fname);
    EXPECT_THAT(vec, ::testing::ElementsAre(-100, 500, 0, 777));

    std::remove(fname);
}

std::vector<int> generateRandomVec(size_t n, int min = -1000, int max = 1000)
{
    std::vector<int> v(n);
    std::mt19937 gen(42);
    std::uniform_int_distribution<int> dist(min, max);
    std::generate(v.begin(), v.end(), [&]() { return dist(gen); });
    return v;
}

TEST(BitonicSort, SortsCorrectly_SmallPowerOfTwo)
{
    std::vector<int> data{ 8, 3, 11, 1, 4, 9, 2, 7 };
    std::vector<int> expected = data;
    std::sort(expected.begin(), expected.end());

    auto searcher = createDeviceSearcher();
    cl::Device device;
    try {
        device = searcher->getFirstSuitableDevice();
    } catch (...) {
        GTEST_SKIP() << "Нет доступного OpenCL устройства";
    }

    std::string kernelSource = bs::readKernel("src/bitonicSort_gkernel.cl") + 
                               bs::readKernel("src/bitonicSort_lkernel.cl");

    bitonicSort(data, device, kernelSource);

    ASSERT_EQ(data.size(), expected.size());
    EXPECT_TRUE(std::is_sorted(data.begin(), data.end()));
    EXPECT_EQ(data, expected);
}

TEST(BitonicSortModernized, SameResultAsClassic_OnSmallArray)
{
    auto data1 = generateRandomVec(32);
    auto data2 = data1;

    auto searcher = createDeviceSearcher();
    cl::Device dev;
    try {
        dev = searcher->getFirstSuitableDevice();
    } catch (...) {
        GTEST_SKIP();
    }

    std::string kernelSource = bs::readKernel("src/bitonicSort_gkernel.cl") + 
                               bs::readKernel("src/bitonicSort_lkernel.cl");

    bitonicSort(data1, dev, kernelSource);
    bitonicSort_modernized(data2, dev, kernelSource);

    EXPECT_EQ(data1, data2);
}

TEST(BitonicSort, ThrowsOnInvalidKernel)
{
    auto searcher = createDeviceSearcher();
    cl::Device dev;
    try {
        dev = searcher->getFirstSuitableDevice();
    } catch (...) {
        GTEST_SKIP();
    }

    std::string badKernel = "__kernel void dummy() { python is the best language }";

    std::vector<int> data(16, 0);

    EXPECT_THROW(
        bitonicSort(data, dev, badKernel),
        cl::Error
    );
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
