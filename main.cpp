// main.cpp Multithreading SPI
// c++11
// g++ -std=c++11 -lpthread


#include "inc/linux_spi.h"
#include "inc/linux_Queue.h"
// #include "inc/deviceMap.h"
#include "inc/viewThreadStackLoad.h"

#pragma GCC optimize ("O0")


static C_Linux_SPI              spi_dev1;
// static C_Queue                  queue_spi_Rx;
// static C_Queue                  queue_spi_Tx;
//static C_DeviceMap_SLE          sle7k_dev_map;


/* multi threads object statically created in the compile time. 
They are not allowed to be created in the heap dynamically */
static C_SPI_Rx_Tx_thread          SPI_Rx_Tx_thread_i(-111, 12);
// static C_SPI_QUEUE_thread          SPI_Queue_thread_i(-69, 27);
// static C_GUI_thread                GUI_thread_i(-54, 36);



     
__attribute__((noinline)) C_Linux_SPI     *          getSPI_dev1(void)   { return (&spi_dev1);     }
// __attribute__((noinline)) C_Queue         *          getRxQueueSPI(void) { return (&queue_spi_Rx); }
// __attribute__((noinline)) C_Queue         *          getTxQueueSPI(void) { return (&queue_spi_Tx); }
//__attribute__((noinline)) C_DeviceMap_SLE *          getDeviceMap(void)  { return (&sle7k_dev_map);}



__attribute__((noinline)) int main()
{
     init_spi_dev(&spi_dev1);

    /* init threads */
     SPI_Rx_Tx_thread_i.init(STACK_SIZE_PTHREAD, Multi_pthread_Prio_List_t::SPI_RX_RX_PTHREAD_PRIO);
    //  SPI_Queue_thread_i.init(STACK_SIZE_PTHREAD, Multi_pthread_Prio_List_t::SPI_QUEUEHANDLER_PTHREAD_PRIO);
    //  GUI_thread_i.init(USE_DEFAULT_STACK_SIZE, Multi_pthread_Prio_List_t::GUI_PRIO);
     
    /* run threads */
     SPI_Rx_Tx_thread_i.start();
    //  SPI_Queue_thread_i.start();
    //  GUI_thread_i.start();
     
     /* Wait / spin()  until threads to be completed */
     SPI_Rx_Tx_thread_i.join();
    //  SPI_Queue_thread_i.join();   
    //  GUI_thread_i.join();   
     

   
     //   view_pthreadsInfo(&SPI_Rx_Tx_thread_i, &SPI_Queue_thread_i);

    // for (int k = 0; k < 9; ++k) {
    //     SPI_Rx_Tx_thread_i.exec();
    //     // cout << "\t\t Counter = " << k << endl;
    //     printf("\t\t Counter = %d \n", k);
    //  }

    while (LOOP_FOR_GOOD)
    {
        ;        
    }  
     spi_dev1.dev_close();  
     
     return 0;  // EXIT_SUCCESS;
}