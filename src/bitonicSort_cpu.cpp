#include <iostream>
#include <vector>
#include <algorithm>

// I got it from https://sortvisualizer.com/bitonicsort/

void bitonicSort(std::vector<int>& arr)
{
    int n = arr.size();
    
    for (int k = 2; k <= n; k *= 2)
        for (int j = k / 2; j > 0; j /= 2)
            for (int i = 0; i < n; i++)
            {
                int l = i ^ j;
                
                if (l > i) {
                    if (((i & k) == 0 && arr[i] > arr[l]) || 
                        ((i & k) != 0 && arr[i] < arr[l])) {
                        std::swap(arr[i], arr[l]);
                    }
                }
            }
}

int main() {
    std::vector<int> arr = {7, 3, 4, 8, 6, 2, 1, 5};

    
    
    bitonicSort(arr);
    
    for (int x : arr) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    
    return 0;
}