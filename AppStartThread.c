/*
 * AppStartThread.c
 *
 *  Created on: 04.12.2013
 *      Author: small
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <ccblkfn.h>
#include <app_cfg.h>
#include <os.h>
#include <adi_types.h>
#include <services/gpio/adi_gpio.h>
#include "SINS_RTOS_CoreA.h"
#include "AppSPORTThread.h"
#include "AppUARTThread.h"

extern SystemParameters system_parameters;

/* ------------------ Globals------------------ */
TaskGlobals AppTaskSPORTGlobals;
TaskGlobals AppTaskUARTGlobals;

void AppStartThread(void* arg)
{
	OS_ERR err;


	/* Create the SPORT task*/
	OSTaskCreate (&(AppTaskSPORTGlobals.TaskTCB), 		/* address of TCB */
			"SINS_RTOS_SPORT",							/* Task name */
			AppSPORTThread,								/* Task main function */
			NULL,										/* argument for the "Run" function*/
			5,											/* priority */
			AppTaskSPORTGlobals.TaskStack,				/* base of the stack */
			TASK_STK_SIZE - 1u ,						/* limit for stack growth */
			TASK_STK_SIZE,								/* stack size in CPU_STK */
			0u,											/*number of messages allowed */
			(OS_TICK)  0u,								/* time_quanta */
			NULL,										/* extension pointer */
			(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
			&err);
	if (err != OS_ERR_NONE){
		printf(" Error creating SPORT thread");
		while(1){ ; }
	}

	/* Create the UART task*/
	OSTaskCreate (&(AppTaskUARTGlobals.TaskTCB), 		/* address of TCB */
			"SINS_RTOS_UART",							/* Task name */
			AppUARTThread,								/* Task main function */
			NULL,										/* argument for the "Run" function*/
			5,											/* priority */
			AppTaskUARTGlobals.TaskStack,				/* base of the stack */
			TASK_STK_SIZE - 1u ,						/* limit for stack growth */
			TASK_STK_SIZE,								/* stack size in CPU_STK */
			10u,										/*number of messages allowed */
			(OS_TICK)  0u,								/* time_quanta */
			NULL,										/* extension pointer */
			(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
			&err);
	if (err != OS_ERR_NONE){
		printf(" Error creating UART thread");
		while(1){ ; }
	}

	while (DEF_ON){
		OSTimeDlyHMSM((CPU_INT16U)0,
				(CPU_INT16U)0,
				(CPU_INT16U)1,
				(CPU_INT16U)0,
				OS_OPT_TIME_HMSM_STRICT, &err);
				if (err != OS_ERR_NONE) {
					printf("Error OSTimeDlyHMSM\n");
					while(1){ ; }
				}
				ADI_GPIO_RESULT res_gpio = adi_gpio_Toggle(ADI_GPIO_PORT_F0, ADI_GPIO_PIN_5);
				if (res_gpio != ADI_GPIO_SUCCESS){
					printf("GPIO Toggle Error\n");
					while(1){ ; }
				}
		OSTaskQPost(&(AppTaskUARTGlobals.TaskTCB), (void *)&system_parameters,
				sizeof(system_parameters), OS_OPT_POST_FIFO, &err);
		if (err != OS_ERR_NONE) {
			printf("Error Posting  Message to UART Thread Queue\n");
			while(1){ ; }
		}

	}
}


