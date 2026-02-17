void compareAndSwap_local(__local int* arr, int i, int j, int dir) {
    if ((arr[i] > arr[j] && dir == 1) || (arr[i] < arr[j] && dir == 0)) {
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

__kernel void bitonicStep_lkernel(__global int* arr, 
                          int stage,
                          __local int* locarr,
                          int n)
{
    int globalId = get_global_id(0);
    int localId = get_local_id(0);
    int groupId = get_group_id(0);
    int localSize = get_local_size(0);
    
    locarr[localId] = arr[globalId];

    barrier(CLK_LOCAL_MEM_FENCE);
    
    int dir = (groupId % 2 == 0) ? 0 : 1;
    
    for (int subStage = stage / 2; subStage > 0; subStage /= 2)
    {
        int l = localId ^ subStage;
        
        if (l < localSize)
        {
            int pairDir = dir;
            
            if ((localId & subStage) == 0)
            {
                pairDir = !dir;
            }
            
            compareAndSwap_local(locarr, localId, l, pairDir);
        }
        
        barrier(CLK_LOCAL_MEM_FENCE);
    }
    
    arr[globalId] = locarr[localId];   
}