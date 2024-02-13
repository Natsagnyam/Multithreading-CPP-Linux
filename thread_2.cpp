#include "inc/deviceMap.h"
#include "inc/pthreadClass.h"
#include "inc/linux_Queue.h"   
#include "inc/linux_spi.h"


  




// extern C_DeviceMap_SLE          sle7k_dev_map;  // No extern allowed. Search for without extern 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; /* locking mechanism for Sle7k_dev, Device_Map */

// The function to be executed by  thread1 
__attribute__((noinline)) int C_SPI_QUEUE_thread::exec()
{
    C_DeviceMap_SLE * ptr_sle7k_dev_map;

     // use arg1_, arg2_....
    printf(" C_SPI_QUEUE_thread arg1_ = %d; \n", arg1_);
    printf(" C_SPI_QUEUE_thread arg2_ = %d; \n", arg2_);
   
    ptr_sle7k_dev_map = (C_DeviceMap_SLE * ) getDeviceMap();   // done without extern 
   
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < SLE_THREAD_DATA_SIZE; ++i)   {
        ptr_sle7k_dev_map->m_thread2[i] = (unsigned char) i + 'A';
        printf("\tThread1 runing[%d] = %c; \n", i, ptr_sle7k_dev_map->m_thread2[i]);
    }   
    for (int i = 0; i < SLE_TOTAL_THREAD_DATA_NUMBER; ++i)   {        
        printf("\tThread array[%d] = %c; \n", i, ptr_sle7k_dev_map->m_array[i]);
    }   
    pthread_mutex_unlock(&mutex);
    


    


    return EXIT_SUCCESS;
}
