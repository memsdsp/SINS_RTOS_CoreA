/*****************************************************************************
 * SINS_RTOS_CoreA.c
 *****************************************************************************/

#include <stdio.h>
#include <stddef.h>
#include <ccblkfn.h>
#include <app_cfg.h>
#include <os.h>
#include <adi_types.h>
#include "adi_initialize.h"
#include "SINS_RTOS_CoreA.h"
#include "InitPower.h"
#include "AppStartThread.h"
#include "GPIOServices.h"

//Global Threads Stack Data
TaskGlobals AppTaskStartGlobals;
TaskGlobals AppTaskSPORTGlobals;
TaskGlobals AppTaskUARTGlobals;

int main(void)
{
	OS_ERR err;
	int32_t result;

	/**
	 * Initialize managed drivers and/or services that have been added to 
	 * the project.
	 * @return zero on success 
	 */
	adi_initComponents();

	adi_core_b_enable();

	/* Initialize Power */
	InitPower();

	//Initialize GPIO Service to enable FPGA data ready semaphore
	InitGPIOServices();

	/* Create the boot task*/
	OSTaskCreate (&( AppTaskStartGlobals.TaskTCB), 		/* address of TCB */
			"SINS_RTOS_Boot",							/* Task name */
			AppStartThread,								/* Task main function */
			NULL,										/* argument for the "Run" function*/
			6,											/* priority */
			AppTaskStartGlobals.TaskStack,				/* base of the stack */
			TASK_STK_SIZE - 1u ,						/* limit for stack growth */
			TASK_STK_SIZE,								/* stack size in CPU_STK */
			0u,											/*number of messages allowed */
			(OS_TICK)  0u,								/* time_quanta */
			NULL,										/* extension pointer */
			(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
			&err);
	if (err != OS_ERR_NONE){
		printf("Error creating boot thread");
		while(1){ ; }
	}

	OSStart(&err);
	if (err != OS_ERR_NONE){
		printf("Error starting the RTOS");
		while(1){ ; }
	}

	return 0;
}

