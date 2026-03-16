.PHONY: linux windows all clean

linux:
	@echo "Creating biton:linux image..."
	@docker build -f docker/Dockerfile.linux -t biton:linux . > /dev/null 2>&1
	@echo "Ok"

	@echo "Creating biton-linux-tmp container..."
	@docker create --name biton-linux-tmp biton:linux > /dev/null 2>&1
	@echo "Ok"

	@echo "Copy binary from container to your system..."
	@mkdir -p build > /dev/null 2>&1
	@docker cp biton-linux-tmp:/src/build/biton ./build/biton > /dev/null 2>&1
	@echo "Ok"

	@echo "Deleting biton-linux-tmp container..."
	@docker rm biton-linux-tmp > /dev/null 2>&1
	@echo "Ok"

	@echo "Run on linux: ./build/biton"

windows:
	@echo "Downloading OpenCL for windows (OpenCL-SDK)..."
	@mkdir -p vendor > /dev/null 2>&1
	@curl -L https://github.com/KhronosGroup/OpenCL-SDK/releases/download/v2025.07.23/OpenCL-SDK-v2025.07.23-Win-x64.zip \
	      -o vendor/OpenCL-SDK-Win-x64.zip > /dev/null 2>&1
	@echo "Ok"

	@echo "Creating biton:windows image..."
	@docker build -f docker/Dockerfile.windows -t biton:windows . > /dev/null 2>&1
	@echo "Ok"

	@echo "Creating biton-win-tmp container..."
	@docker create --name biton-win-tmp biton:windows > /dev/null 2>&1
	@echo "Ok"

	@echo "Copy exe from container to your system..."
	@mkdir -p build > /dev/null 2>&1
	@docker cp biton-win-tmp:/src/build-win/biton.exe ./build/biton.exe > /dev/null 2>&1
	@echo "Ok"

	@echo "Deleting biton-win-tmp container..."
	@docker rm biton-win-tmp > /dev/null 2>&1
	@echo "Ok"

	@echo 'Run on windows: build\biton.exe'

all: linux windows
	@echo "All executables in build/"

clean:
	@docker rm -f biton-linux-tmp biton-win-tmp 2>/dev/null || true
	@rm -rf build/ vendor/
