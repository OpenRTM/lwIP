#ifndef LWIP_ARCH_SYS_ARCH_H
#define LWIP_ARCH_SYS_ARCH_H
#include "kernel.h"
#include "lwip/opt.h"
#include "lwip/arch.h"

#define portTICK_PERIOD_MS 1000
#define configTICK_RATE_Hz 100
#define portTICK_RATE_NS   ()

typedef ID sys_sem_t;
typedef ID sys_mbox_t;
typedef ID sys_thread_t;
typedef ID sys_mutex_t;
typedef u32_t sys_prot_t;
#define sys_mbox_valid( x ) ( ( ( *x ) == NULL) ? false : true )
#define sys_mbox_set_invalid( x ) ( ( *x ) = NULL )
#define sys_sem_valid( x ) ( ( ( *x ) == NULL) ? false : true )
#define sys_sem_set_invalid( x ) ( ( *x ) = NULL )
typedef void (*thread_func)(void *thread_arg);
extern void lwip_task(EXINF exinf);
#endif
