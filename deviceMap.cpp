
#include "inc/deviceMap.h"
#include "inc/sle_def.h"




unsigned char * C_DeviceMap_SLE::getMapThread(threadID_t id) 
{ 
    unsigned char * ptr_char = nullptr;
    switch (id)
    {
    case multi_pthread_ID_List::SPI_RX_PTHREAD:
        /* code */
        // ptr_char = &DeviceMap::AllThreadsMAP::thread1[0];
        ptr_char = DeviceMap::AllThreadsMAP::thread1;   
        //ptr_char = m_thread1;
        break;
    case multi_pthread_ID_List::SPI_TX_PTHREAD:
        /* code */
        ptr_char = DeviceMap::AllThreadsMAP::thread2;
        //ptr_char = m_thread2;
        break;        
    case multi_pthread_ID_List::SPI_RXQUEUEHANDLER_PTHREAD:
        /* code */
        ptr_char = DeviceMap::AllThreadsMAP::thread2;
        //ptr_char = m_thread3;
        break;        
    
    default:
        break;
    }
    return (unsigned char *) (ptr_char);
}