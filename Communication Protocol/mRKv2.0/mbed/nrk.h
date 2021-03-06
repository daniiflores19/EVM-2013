/******************************************************************************
*  Nano-RK, a real-time operating system for sensor networks.
*  Copyright (C) 2007, Real-Time and Multimedia Lab, Carnegie Mellon University
*  All rights reserved.
*
*  This is the Open Source Version of Nano-RK included as part of a Dual
*  Licensing Model. If you are unsure which license to use please refer to:
*  http://www.nanork.org/nano-RK/wiki/Licensing
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, version 2.0 of the License.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*  Contributing Authors (specific to this file):
*  Anthony Rowe
*  Zane Starr
*******************************************************************************/


#ifndef __nrk_h    /* Only include stuff once */
#define __nrk_h

//#define NRK_KERNEL_STK_TOP  (RAMEND-1)
#define NRK_KERNEL_STK_TOP  (0x10008000 -1)
#define  NRK_VERSION              101
#define __AVR_LIBC_DEPRECATED_ENABLE__ 1

#include <nrk_cpu.h> 
//#include <nrk_cfg.h>
#include <nrk_time.h>
#include <nrk_task.h>
#include <nrk_events.h>

#ifndef  FALSE
#define  FALSE                     0
#endif

#ifndef  TRUE
#define  TRUE                      1
#endif


/***********************************************************
*               FROM NRK_DEFS.H
***********************************************************/
#define SWAP_BYTES(x)\
    ((((uint16_t)x)<<8)|((((uint16_t)x)>>8)&0xff))

#ifndef NRK_MAX_RESOURCE_CNT
#define NRK_MAX_RESOURCE_CNT 0
#endif

#ifdef NRK_KERNEL_TEST
    extern uint8_t nrk_max_sleep_wakeup_time;
#endif

#ifndef NRK_IDLE_TASK_ID
#define NRK_IDLE_TASK_ID    0
#endif

/***********************************************************
*                       CORE SERVICES           *
***********************************************************/

void    nrk_init(void);
void     nrk_start(void);
//extern "C" void ENABLE_GLOBAL_INT();
//extern "C" void DISABLE_GLOBAL_INT();
void nrk_halt();
void nrk_int_disable(void);
void nrk_int_enable(void);
extern "C" void _nrk_timer_tick(void);
extern "C" void _startos();
void nrk_task_set_stk( nrk_task_type *task, NRK_STK stk_base[], uint16_t stk_size );
// had to change function signature from "void *func" to "void (*func) (void)" for c++ compiler
void nrk_task_set_entry_function( nrk_task_type *task, void (*func) (void));

void    *nrk_task_stk_init(void (*)(), NRK_STK *, NRK_STK *);

extern NRK_STK nrk_idle_task_stk[NRK_TASK_IDLE_STK_SIZE];  /* Idle task stack   */
#ifdef KERNEL_STK_ARRAY
    extern NRK_STK nrk_kernel_stk[NRK_KERNEL_STACKSIZE];
#endif
extern NRK_STK *nrk_kernel_stk_ptr;


int8_t     nrk_TCB_init(nrk_task_type *, NRK_STK *, NRK_STK *, uint16_t , void *, uint16_t);



extern NRK_TCB    nrk_task_TCB[NRK_MAX_TASKS];   /* Table of TCBs */

extern nrk_sig_t nrk_wakeup_signal;

extern nrk_queue    _nrk_readyQ[NRK_MAX_TASKS+1];
extern nrk_queue    *_free_node,*_head_node;




/*--------- Running task context --------*/

extern uint8_t     nrk_cur_task_prio;
extern NRK_TCB    *nrk_cur_task_TCB;

extern uint8_t    nrk_high_ready_prio;
extern NRK_TCB    *nrk_high_ready_TCB;
extern uint32_t	task_addr;	
//Resource Management variables exported 
extern uint8_t  _nrk_resource_cnt;
extern uint8_t    nrk_resource_list[NRK_MAX_RESOURCE_CNT];
extern nrk_sem_t nrk_sem_list[NRK_MAX_RESOURCE_CNT];
extern uint8_t    nrk_resource_prio_list[NRK_MAX_RESOURCE_CNT]; /*directly corresponds to the nrk_resource_list*/

//extern int32_tU   OSTime;
//
extern nrk_time_t nrk_system_time;
extern "C" void _swap(void); 
//void _nrk_setup_timer_test();
//void _nrk_os_timer_start_test();
//void _nrk_os_timer_reset_test();


//extern uint32_t  OSIdleCtr;     /* Idle counter    */


#endif

