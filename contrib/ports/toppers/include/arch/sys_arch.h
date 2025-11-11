#ifndef LWIP_ARCH_SYS_ARCH_H
#define LWIP_ARCH_SYS_ARCH_H
#include "lwip/opt.h"
#include "lwip/arch.h"

#define portTICK_PERIOD_MS 1000
#define configTICK_RATE_Hz 100
#define portTICK_RATE_NS   ()
typedef signed int ID;
typedef ID sys_sem_t;
typedef ID sys_mbox_t;
typedef ID sys_thread_t;
typedef ID sys_mutex_t;
typedef u32_t sys_prot_t;
typedef intptr_t EXINF;
#define false 0
#define true  1
#define sys_mbox_valid( x ) ( ( ( *x ) == NULL) ? false : true )
#define sys_mbox_set_invalid( x ) ( ( *x ) = NULL )
#define sys_sem_valid( x ) ( ( ( *x ) == NULL) ? false : true )
#define sys_sem_set_invalid( x ) ( ( *x ) = NULL )

void lwip_task(EXINF exinf);
int sys_thread_done(sys_thread_t *thread);
#endif

