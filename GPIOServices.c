/*
 * GPIOServices.c
 *
 *  Created on: 05.12.2013
 *      Author: small
 */

#include <services/gpio/adi_gpio.h>
#include <services/int/adi_int.h>
#include <stdio.h>
#include "SINS_RTOS_CoreA.h"

#define GPIO_MEMORY_SIZE (ADI_GPIO_CALLBACK_MEM_SIZE*2)
#define FPGA_DATA_RDY_FLAG ADI_GPIO_PIN_0
#define FPGA_PROG_FLAG     ADI_GPIO_PIN_3
#define FPGA_DONE_FLAG     ADI_GPIO_PIN_4

extern TaskGlobals AppTaskSPORTGlobals;

static uint8_t gpioMemory[GPIO_MEMORY_SIZE];


void FPGADataRdyCallback(ADI_GPIO_PORT ePort, uint32_t Data, void *pCBParam)
{
	OS_ERR  err;

	if (ePort ==  ADI_GPIO_PORT_F0)
	{
		if (Data & FPGA_DATA_RDY_FLAG)
		{
			OSTaskSemPost(&AppTaskSPORTGlobals.TaskTCB, OS_OPT_POST_NONE, &err);
	        if (err != OS_ERR_NONE)
	        {
	            printf("Error Posting Semaphore \n");
	            while(1){ ; }
	        }

		}
	}

}

void InitGPIOServices (void)
{
	uint32_t gpioMaxCallbacks;

	// init the GPIO service
	ADI_GPIO_RESULT result = adi_gpio_Init(
			(void*)gpioMemory,
			GPIO_MEMORY_SIZE,
			&gpioMaxCallbacks);
	if (result != ADI_GPIO_SUCCESS){
		printf("GPIO Init Error\n");
		while(1){ ; }
	}

	result = adi_gpio_SetDirection(
			ADI_GPIO_PORT_F0,
			FPGA_DATA_RDY_FLAG,
			ADI_GPIO_DIRECTION_INPUT);
	if (result != ADI_GPIO_SUCCESS){
		printf("GPIO SetDirection Error\n");
		while(1){ ; }
	}

	result = adi_gpio_SetDirection(
			ADI_GPIO_PORT_F0,
			ADI_GPIO_PIN_7,
			ADI_GPIO_DIRECTION_OUTPUT);
	if (result != ADI_GPIO_SUCCESS){
		printf("GPIO SetDirection Error\n");
		while(1){ ; }
	}

	result = adi_gpio_SetDirection(
			ADI_GPIO_PORT_F0,
			ADI_GPIO_PIN_6,
			ADI_GPIO_DIRECTION_OUTPUT);
	if (result != ADI_GPIO_SUCCESS){
		printf("GPIO SetDirection Error\n");
		while(1){ ; }
	}

	result = adi_gpio_SetDirection(
			ADI_GPIO_PORT_F0,
			ADI_GPIO_PIN_5,
			ADI_GPIO_DIRECTION_OUTPUT);
	if (result != ADI_GPIO_SUCCESS){
		printf("GPIO SetDirection Error\n");
		while(1){ ; }
	}

	result = adi_gpio_SetDirection(
			ADI_GPIO_PORT_F0,
			FPGA_PROG_FLAG,
			ADI_GPIO_DIRECTION_OUTPUT);
	if (result != ADI_GPIO_SUCCESS){
		printf("GPIO SetDirection Error\n");
		while(1){ ; }
	}
	adi_gpio_Clear(ADI_GPIO_PORT_F0, FPGA_PROG_FLAG);

	result = adi_gpio_SetDirection(
			ADI_GPIO_PORT_F0,
			FPGA_DONE_FLAG,
			ADI_GPIO_DIRECTION_INPUT);
	if (result != ADI_GPIO_SUCCESS){
		printf("GPIO SetDirection Error\n");
		while(1){ ; }
	}

	result = adi_gpio_SetInputEdgeSense(
			ADI_GPIO_PORT_F0,
			FPGA_DATA_RDY_FLAG,
			ADI_GPIO_SENSE_RISING_EDGE);
	if (result != ADI_GPIO_SUCCESS){
		printf("GPIO SetInputEdgeSense Error\n");
		while(1){ ; }
	}


	result = adi_gpio_EnableInterruptMask(
			ADI_GPIO_PORT_F0,
			FPGA_DATA_RDY_FLAG,
			ADI_GPIO_MASK_A,
			true);
	if (result != ADI_GPIO_SUCCESS){
		printf("GPIO EnableInterruptMask Error\n");
		while(1){ ; }
	}

	// register FPGADataRdyCallback
	result = adi_gpio_RegisterCallback(
			ADI_GPIO_PORT_F0,
			FPGA_DATA_RDY_FLAG,
			FPGADataRdyCallback,
			(void*)0);
	if (result != ADI_GPIO_SUCCESS){
		printf("GPIO RegisterCallback Error\n");
		while(1){ ; }
	}

	uint32_t port_value;
	uint32_t flag_value = false;

	//Start FPGA Program Sequence
	adi_gpio_Set(ADI_GPIO_PORT_F0, FPGA_PROG_FLAG);

	//Test if FPGA Program is done
	while (!flag_value){
		adi_gpio_GetData(ADI_GPIO_PORT_F0, &port_value);
		flag_value = port_value & FPGA_DONE_FLAG;
	}

}

