/* posix thread in c++ */
/* No heap allowed = No inline function allowed + No malloc allowed + No new operator allowed */

//#include "inc/deviceMap.h"
#include "inc/pthreadClass.h"






__attribute__((noinline))void* pthread::thread_router(void* arg)
{
    reinterpret_cast<pthread*>(arg)->exec_thread();
    return NULL;
}
 
__attribute__((noinline))bool pthread::init(size_t my_stack_size, Multi_pthread_Prio_List_t prio)
{ 
    m_priority = prio;
    m_stack_size = my_stack_size;

    if (my_stack_size == USE_DEFAULT_STACK_SIZE) {
    pthread_attr_init(&m_tattr); /* initialized with default attributes */ 
    } else { 
    m_stackbase = &m_arr; 
    pthread_attr_setstack(&m_tattr, m_stackbase, m_stack_size); /* m_stackbase points where stack is */
    // pthread_attr_setstackaddr(&m_tattr, m_stackbase); /* set stack base address */ 
    // pthread_attr_setstacksize(&m_tattr, m_stack_size); /* set stack size */ 
    pthread_attr_init(&m_tattr); /* initialized with the new attributes */
    }
    pthread_setschedprio(m_threads_ID, prio); /* set pthread priority */ 
    return 0;       // EXIT_SUCCESS;
}


