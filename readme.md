## Сборка

```bash
cmake -S . -B build
cmake --build build
# Запус битонической сортировки 
./build/biton tests/test2.dat 

# Запуск сравнения std::sort и моей bitonicSort
./build/compare tests/test1.dat 
```

## Установка opencl

```bash
sudo apt update

sudo apt install ocl-icd-libopencl1

sudo apt install pocl-opencl-icd

# Установите OpenCL для Intel GPU
sudo apt install intel-opencl-icd

# Установите OpenCL для CPU
sudo apt install pocl-opencl-icd
```

ls /etc/OpenCL/vendors/ - находим какие устройства видны и поддерживаются opencl



