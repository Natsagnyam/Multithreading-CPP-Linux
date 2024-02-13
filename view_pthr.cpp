#include "inc/viewThreadStackLoad.h"
// #include <iostream>
#include <stdio.h>
//using namespace std;


__attribute__((noinline)) void print_sle_thread_stack(unsigned char * char_ptr)
{
    // cout << "\t\t\t\t Thread stack[] = ?" << endl;
    printf( "\t\t\t\t Thread stack[] = ?");
    for (int i = 0; i < 15; ++i)  {
        printf(" Line[%d] = stack[%s] = %c; \n", i, char_ptr, *char_ptr);
        char_ptr--;
    }


}


__attribute__((noinline)) void view_pthreadsInfo(C_SPI_Rx_Tx_thread *pt1, C_SPI_QUEUE_thread *pt2)
{
    unsigned char * char_ptr;
     char_ptr = pt1->get_stack_location();
     // cout << " C_SPI_Rx_Tx_thread stack size = " <<  pt1->get_stack_size() << endl;
     printf(" C_SPI_Rx_Tx_thread stack size = %ld; \n", pt1->get_stack_size());
     printf(" C_SPI_Rx_Tx_thread stack location = %s", char_ptr);

     printf(" C_SPI_Rx_Tx_thread stack location = %s; \n", char_ptr);
     print_sle_thread_stack(char_ptr);
          
     char_ptr = pt2->get_stack_location();
     // cout << " \t\tC_SPI_Tx_thread stack size = " <<  pt2->get_stack_size() << endl;
     printf(" \t\tC_SPI_Tx_thread stack size = %ld; \n", pt2->get_stack_size());
     // cout << " \t\tC_SPI_Tx_thread stack location = " <<  char_ptr << endl;
     printf(" \t\tC_SPI_Tx_thread stack location = %s; \n", char_ptr);
     printf(" \t\tC_SPI_Tx_thread stack location = %s; \n", char_ptr);
     print_sle_thread_stack(char_ptr);
}
