
#include "inc/linux_Queue.h"
#include "inc/sle_def.h"
                           


	bool C_Queue::PushByte2Queue(int in) { // add element at the end of queue
        if (!isQueueFull()) {
        queue[in_ptr] = in;
        ++in_ptr;
        in_ptr &= QUEUE_BUFFER_SIZE_SPI_RX_TX;          // circular buffer
        return EXIT_SUCCESS_;
        }  else  { 
            //cout << "Error:Queue full" << endl; 
            printf("Error:Queue full\n");
            return EXIT_FAILURE_; 
        }
	}
	int C_Queue::PopByteFromQueue(void) { // return and remove 1st element from queue
        int temp; 
		if (!isQueueEmpty()) {
            temp = queue[out_ptr]; 
            ++out_ptr;
            out_ptr &= QUEUE_BUFFER_SIZE_SPI_RX_TX;          // circular buffer
            return temp;
        } else  { 
            //cout << "Error:Queue empty" << endl; 
            printf("Error:Queue empty\n");
            return (EXIT_FAILURE_ * (-1)); 
        }
    }
	bool C_Queue::isQueueEmpty(void)
    {
        if (out_ptr == in_ptr) { return true; }
        else { return false; }
    }
	bool C_Queue::isQueueFull()
    {
        if ((out_ptr + 1) == in_ptr) { return true; }
        else { return false; }
    }    
    void C_Queue::resetQueue(void)  // empty the queue
    {
        in_ptr = __NULL_;
        out_ptr = __NULL_;
    }	
    
    // add array at the end of queue
    __attribute__((noinline)) bool C_Queue::addArray2queue(unsigned char *arr, int len) {                              
        if (!isQueueFull()) {
            for (int i = 0; i < len; ++i )  {
                queue[in_ptr] = arr[i];
                ++in_ptr;
                in_ptr &= QUEUE_BUFFER_SIZE_SPI_RX_TX;          // circular buffer   
                if (isQueueFull()) {   resetQueue();  return EXIT_FAILURE_;  }          
            }
            return EXIT_SUCCESS_;
            }  else  { 
                printf("Error:Queue full\n");
                return EXIT_FAILURE_; 
            }      
	}
    bool C_Queue::getArrayFromQueue(int *arr, __int16_t len) { // return and remove 1st element from queue        
		if (!isQueueEmpty()) {
            for (int i = 0; i < len; ++i )  {
                arr[i] = queue[out_ptr]; 
                ++out_ptr;
                out_ptr &= QUEUE_BUFFER_SIZE_SPI_RX_TX;          // circular buffer
                if (isQueueEmpty()) {   resetQueue();  return EXIT_FAILURE_;  }                  
            }
            return EXIT_SUCCESS_;
        }  else  { 
            printf("Error:Queue empty\n");
            return EXIT_FAILURE_; 
        }
    };
    void * C_Queue::runRx(void)   {
        int x = 0;
        ++x;
    }    
    __attribute__((noinline)) void *C_Queue::cRun_Rx(void *p) 
    {
      return reinterpret_cast<C_Queue*>(p)->runRx();
    }

    void * C_Queue::runTx(void)   {
        ;
    }
    void *C_Queue::cRun_Tx(void *p) 
    {
      return reinterpret_cast<C_Queue*>(p)->runTx();
    }

    void * C_Queue::handling(void)   {
        ;
    }
    void *C_Queue::cRun_Handling(void *p) 
    {
      return reinterpret_cast<C_Queue*>(p)->handling();
    }    



    
