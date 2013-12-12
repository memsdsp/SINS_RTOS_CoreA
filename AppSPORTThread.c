/*
 * AppSPORTThread.c
 *
 *  Created on: 04.12.2013
 *      Author: small
 */

#include <stdio.h>
#include <stddef.h>
#include <string.h>
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
#define CHANNELS_NUMBER 8

/*-------------------Externals------------------------*/
extern OS_MUTEX   MutexUARTSend;
extern float adc_decimated_channels[8];

/* SPORT driver memory */
uint8_t DeviceMemory[ADI_SPORT_DMA_MEMORY_SIZE];

/* Tx buffers-0 */
uint8_t SPORTRxBuffer[SIZE_OF_RX_BUFFER];

static float dec_channels[CHANNELS_NUMBER];


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

	static uint8_t dec_cnt = 0;
	uint8_t i;

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
		if (err != OS_ERR_NONE) {
			printf("Error Pending on Semaphore \n");
			while(1){ ; }
		}

		eResult = adi_sport_GetBuffer(hDevice,&pProcessedBuffer);
		if (eResult != ADI_SPORT_SUCCESS){
			printf("Sport GetBuffer Error\n");
			while(1){;}
		} else {
			//Decimate Incoming data
			uint32_t *pChannel;
			pChannel = (uint32_t *)SPORTRxBuffer;
			for (i=0; i<CHANNELS_NUMBER; i++){
				dec_channels[i] += pChannel[i];
			}
			if (dec_cnt++ == 2){
				for (i=0; i<CHANNELS_NUMBER; i++){
					dec_channels[i] = dec_channels[i]/3.0f;
					dec_cnt = 0;
				}

		        /* Access shared resource */
		        OSMutexPend((OS_MUTEX  *)&MutexUARTSend,
		        		(OS_TICK    )0,
		        		(OS_OPT     )OS_OPT_PEND_BLOCKING,
		        		(CPU_TS    *)0,
		        		(OS_ERR    *)&err);
				if (err != OS_ERR_NONE) {
					printf("Error Pending on Mutex\n");
					while(1){ ; }
				}
				//copy decimated adc channels data to the global buffer
				memcpy(adc_decimated_channels, dec_channels, sizeof(float)*CHANNELS_NUMBER);
		        OSMutexPost((OS_MUTEX  *)&MutexUARTSend,
		        		(OS_OPT     )OS_OPT_POST_NONE,
		        		(OS_ERR    *)&err);
				if (err != OS_ERR_NONE) {
					printf("Error Posting Mutex\n");
					while(1){ ; }
				}
				adi_gpio_Toggle(ADI_GPIO_PORT_F0, ADI_GPIO_PIN_7);
				memset(dec_channels, 0, sizeof(float)*CHANNELS_NUMBER);
			}// if (dec_cnt++ == 2)

			/* submit the buffer */
			eResult = adi_sport_SubmitBuffer(hDevice,SPORTRxBuffer,SIZE_OF_RX_BUFFER);
			if (eResult != ADI_SPORT_SUCCESS){
				printf("Sport SubmitBuffer Error\n");
				while(1){ ; }
			}
//			adi_gpio_Toggle(ADI_GPIO_PORT_F0, ADI_GPIO_PIN_7);
		} // if (eResult...) else

	}// while (DEF_ON)

}

