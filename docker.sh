if [ "$1" = "linux" ]; then

    echo "Creating biton:linux image..."
    docker build -f docker/Dockerfile.linux -t biton:linux . &> /dev/null
    echo "Ok"
    
    echo "Creating biton-linux-tmp container..."
    docker create --name biton-linux-tmp biton:linux &> /dev/null
    echo "Ok"

    echo "Copy exe from container to your system..."
    mkdir -p build &> /dev/null
    docker cp biton-linux-tmp:/src/build/biton ./build/biton &> /dev/null
    echo "Ok"

    echo "Deleting biton-linux-tmp container..."
    docker rm biton-linux-tmp &> /dev/null
    echo "Ok"
    
    echo "Run on linux: ./build/biton"

elif [ "$1" = "windows" ]; then

    echo "Downloading OpenCL for windows (OpenCL-SDK)..."
    mkdir -p vendor &> /dev/null
    curl -L https://github.com/KhronosGroup/OpenCL-SDK/releases/download/v2025.07.23/OpenCL-SDK-v2025.07.23-Win-x64.zip -o vendor/OpenCL-SDK-Win-x64.zip &> /dev/null
    echo "Ok"

    echo "Creating biton:windows image..."
    docker build -f docker/Dockerfile.windows -t biton:windows . &> /dev/null
    echo "Ok"
    
    echo "Creating biton-win-tmp container..."
    docker create --name biton-win-tmp biton:windows &> /dev/null
    echo "Ok"

    echo "Copy exe from container to your system..."
    mkdir -p build &> /dev/null
    docker cp biton-win-tmp:/src/build-win/biton.exe ./build/biton.exe &> /dev/null
    echo "Ok"

    echo "Deleting biton-win-tmp container..."
    docker rm biton-win-tmp &> /dev/null
    echo "Ok"

    echo "Run on windows: build\biton.exe"

elif [ "$1" = "all" ]; then
    $0 linux
    $0 windows

    echo "All exe in build"

else
    echo "Usage: $0 [linux|windows|all]"
    exit 1
fi