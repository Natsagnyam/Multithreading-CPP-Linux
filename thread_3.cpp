
#include "inc/deviceMap.h"
#include "inc/pthreadClass.h"

// extern C_DeviceMap_SLE          sle7k_dev_map;  // No extern allowed. Searcj for without extern 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; /* locking mechanism for Sle7k_dev, Device_Map */

// The function to be executed by  thread1 
__attribute__((noinline)) int C_SPI_Rx_Tx_thread::exec()
{
    C_DeviceMap_SLE * ptr_sle7k_dev_map;

     // use arg1_, arg2_....
    cout << " SPI_TXQUEUEHANDLER_PTHREAD arg1_ = " <<  arg1_ << endl;
    cout << " SPI_TXQUEUEHANDLER_PTHREAD arg2_ = " <<  arg2_ << endl;
    
   
   // pthread_mutex_t * ptr_mutex;

    //ptr_mutex = getSLE_mutex_thread_1();
       
	// ptr_sle7k_dev_map = vargp;
    ptr_sle7k_dev_map = (C_DeviceMap_SLE * ) getDeviceMap();   // done without extern 
   
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < SLE_THREAD_DATA_SIZE; ++i)   {
        ptr_sle7k_dev_map->m_thread3[i] = (unsigned char) i + 'A';
        printf("\tThread1 runing[%d] = %c; \n", i, ptr_sle7k_dev_map->m_thread3[i]);
    }   
    for (int i = 0; i < SLE_TOTAL_THREAD_DATA_NUMBER; ++i)   {        
        printf("\tThread array[%d] = %c; \n", i, ptr_sle7k_dev_map->m_array[i]);
    }   
    pthread_mutex_unlock(&mutex);
    
    return EXIT_SUCCESS;
}
