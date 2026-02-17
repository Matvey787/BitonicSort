void compareAndSwap_global(__global int* arr, int i, int j, int dir) {
    if ((arr[i] > arr[j] && dir == 1) || (arr[i] < arr[j] && dir == 0)) {
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

__kernel void bitonicStep_gkernel(__global int* arr, 
                            int stage,
                            int subStage,
                            int dir)
{
    
    int i = get_global_id(0);
    
    int l = i ^ subStage;
    
    if (l > i)
    {
        int pairDir = dir;
        
        if ((i & stage) != 0)
        {
            pairDir = !dir;
        }
        
        compareAndSwap_global(arr, i, l, pairDir);
    }
}