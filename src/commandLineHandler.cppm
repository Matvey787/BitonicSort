module;

#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <limits>

#include "llvm/Support/CommandLine.h"

export module commandLineHandler;

llvm::cl::opt<std::string> InputFile(
    "file",
    llvm::cl::desc("Input file with numbers to sort"),
    llvm::cl::value_desc("filename"),
    llvm::cl::init("")
);

llvm::cl::alias InputFileShort(
    "f",
    llvm::cl::desc("Input file with numbers to sort"),
    llvm::cl::aliasopt(InputFile)
);

llvm::cl::opt<bool> Compare(
    "compare",
    llvm::cl::desc("Compare with std::sort"),
    llvm::cl::init(false)
);

llvm::cl::alias CompareShort(
    "c",
    llvm::cl::desc("Compare with std::sort"),
    llvm::cl::aliasopt(Compare)
);

llvm::cl::opt<bool> ShowDevice(
    "dev",
    llvm::cl::desc("Show selected OpenCL device"),
    llvm::cl::init(false)
);

llvm::cl::opt<bool> ShowAllDevices(
    "shdevs",
    llvm::cl::desc("Show all available OpenCL devices"),
    llvm::cl::init(false)
);

llvm::cl::opt<std::string> SelectDevice(
    "select",
    llvm::cl::desc("Select device by platform and device index (format: <platformIdx>:<deviceIdx>)"),
    llvm::cl::value_desc("platformIdx:deviceIdx"),
    llvm::cl::init("auto")
);


llvm::cl::opt<bool> ShowHelp(
    "h",
    llvm::cl::desc("Show this help message"),
    llvm::cl::init(false)
);


export namespace clh {

struct CommandLineOptions
{
    std::string inputFile;
    bool compare = false;
    bool showDevice = false;
    bool showAllDevices = false;
    std::string selectDevice = "auto";
    bool help = false;
};

CommandLineOptions parseCommandLine(int argc, const char* argv[]);

} // namespace clh

namespace clh {

CommandLineOptions parseCommandLine(int argc, const char* argv[])
{
    llvm::cl::ParseCommandLineOptions(argc, argv, "Bitonic sort using OpenCL\n");

    CommandLineOptions options;
    options.help = ShowHelp;
    options.inputFile = InputFile;
    options.compare = Compare;
    options.showDevice = ShowDevice;
    options.showAllDevices = ShowAllDevices;
    options.selectDevice = SelectDevice;

    if (options.help)
    {
        llvm::cl::PrintHelpMessage();
        std::exit(0);
    }

    return options;
}

} // namespace clh
