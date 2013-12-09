/*
 * AppSPORTThread.c
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
#include <drivers/sport/adi_sport.h>
#include <services/gpio/adi_gpio.h>
#include "SINS_RTOS_CoreA.h"
#include "GPIOServices.h"

#define SIZE_OF_RX_BUFFER 32

/* SPORT driver memory */
uint8_t DeviceMemory[ADI_SPORT_DMA_MEMORY_SIZE];

/* Tx buffers-0 */
uint8_t SPORTRxBuffer[SIZE_OF_RX_BUFFER];


void AppSPORTThread(void* arg)
{

	/* SPORT driver handle */
	ADI_SPORT_HANDLE hDevice;

	/* driver API result code */
	ADI_SPORT_RESULT  eResult;
	uint32_t HwError;

	void *pProcessedBuffer;

	CPU_TS  ts;
	OS_ERR err;

	/* open the SPORT driver */
	eResult = adi_sport_Open(0,ADI_SPORT_DIR_RX, ADI_SPORT_SERIAL_MODE, DeviceMemory,
			ADI_SPORT_DMA_MEMORY_SIZE,&hDevice);
	if (eResult != ADI_SPORT_SUCCESS){
		printf("Sport Open Error\n");
		while(1){ ; }
	}

	/* Enable the DMA mode */
	eResult =  adi_sport_EnableDMAMode(hDevice,true);
	if (eResult != ADI_SPORT_SUCCESS){
		printf("Sport EnableDMA Error\n");
		while(1){ ; }
	}

	/*
	 * Use the driver API's to configure SPORT
	 *
	 * adi_sport_ConfigData();
	 * adi_sport_ConfigClock();
	 * adi_sport_ConfigFrameSync();
	 */

	//configure data format
	bool		LSBFirst = false;
	uint8_t		WordLength = 31;
	eResult = adi_sport_ConfigData(hDevice,ADI_SPORT_DTYPE_SIGN_FILL, LSBFirst,WordLength);
	if (eResult != ADI_SPORT_SUCCESS){
		printf("Sport ConfigData Error\n");
		exit(-1);
	}
	//configure clock
	uint16_t	ClockRatio = 0;
	bool		UseIntlClock = false;
	bool		FallingEdge = true;
	eResult = adi_sport_ConfigClock(hDevice, ClockRatio, UseIntlClock,  FallingEdge);
	if (eResult != ADI_SPORT_SUCCESS){
		printf("Sport ConfigClock Error\n");
		while(1){ ; }
	}

	//configure frame sync
	uint16_t	FsDivisor = 0;
	bool		FSRequired = true;
	bool		InternalFS = false;
	bool		DataFS = false;
	bool		ActiveLowFS = false;
	bool		LateFS = true;
	eResult = adi_sport_ConfigFrameSync(hDevice, FsDivisor, FSRequired,
			InternalFS, DataFS, ActiveLowFS, LateFS);
	if (eResult != ADI_SPORT_SUCCESS){
		printf("Sport ConfigFrameSync Error\n");
		while(1){ ; }
	}

	/* submit the buffer */
	eResult = adi_sport_SubmitBuffer(hDevice,SPORTRxBuffer,SIZE_OF_RX_BUFFER);
	if (eResult != ADI_SPORT_SUCCESS){
		printf("Sport SubmitBuffer Error\n");
		while(1){ ; }
	}


	/* Enable the device */
	eResult = adi_sport_Enable(hDevice,true);
	if (eResult != ADI_SPORT_SUCCESS){
		printf("Sport Enable Error\n");
		while(1){ ; }
	}

	while (DEF_ON){

		OSTaskSemPend(0, OS_OPT_PEND_BLOCKING, &ts,	&err);
		if (err != OS_ERR_NONE)
		{
			printf("Error Pending on Semaphore \n");
			while(1){ ; }
		}

		eResult = adi_sport_GetBuffer(hDevice,&pProcessedBuffer);
		if (eResult != ADI_SPORT_SUCCESS){
			printf("Sport GetBuffer Error\n");
			while(1){;}
		} else {
			/* submit the buffer */
			eResult = adi_sport_SubmitBuffer(hDevice,SPORTRxBuffer,SIZE_OF_RX_BUFFER);
			if (eResult != ADI_SPORT_SUCCESS){
				printf("Sport SubmitBuffer Error\n");
				while(1){ ; }
			}
			adi_gpio_Toggle(ADI_GPIO_PORT_F0, ADI_GPIO_PIN_7);
		}

	}

}
