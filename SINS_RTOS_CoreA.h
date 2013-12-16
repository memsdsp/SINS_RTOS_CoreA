/*****************************************************************************
 * SINS_RTOS_CoreA.h
 *****************************************************************************/

#ifndef __SINS_RTOS_COREA_H__
#define __SINS_RTOS_COREA_H__

#include <os.h>

#define TASK_STK_SIZE ((CPU_STK)0x400u)
#define TASK_PRIO ((OS_PRIO) 4)

/* Add your custom header content here */
typedef struct
{
	CPU_STK TaskStack[TASK_STK_SIZE];
	OS_TCB  TaskTCB;
} TaskGlobals;

typedef struct
{
	uint32_t   fcclk;
	uint32_t   fsclk;
	uint32_t BaudRate;
} SystemParameters;


#endif /* __SINS_RTOS_COREA_H__ */
