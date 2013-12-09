/*
 * AppUARTThread.c
 *
 *  Created on: 07.12.2013
 *      Author: small
 */

#include <os.h>
#include <drivers/uart/adi_uart.h>
#include <services/gpio/adi_gpio.h>
#include <stdio.h>

/* UART driver memory */
uint8_t driverMemory[ADI_UART_UNIDIR_DMA_MEMORY_SIZE];

void AppUARTThread(void* arg)
{

	OS_ERR err;

	/* Pointer to the buffer */
	uint8_t *pBuffer;

	/* buffer which holds data to transfer over UART */
	uint8_t buffer[] = {1, 2, 3};

	/* UART Driver Handle */
	ADI_UART_HANDLE hDevice;

	/* UART result code */
	ADI_UART_RESULT result;


	/* open the UART driver in Tx mode only */
	result = adi_uart_Open(0, ADI_UART_DIR_TRANSMIT, driverMemory, ADI_UART_UNIDIR_DMA_MEMORY_SIZE, &hDevice);
	if (result != ADI_UART_SUCCESS){
		printf("UART Open Error\n");
		while(1){ ; }
	}

	/* Set DMA Mode */
	result = adi_uart_EnableDMAMode(hDevice, true);
	if (result != ADI_UART_SUCCESS){
		printf("UART EnableDMAMode Error\n");
		while(1){ ; }
	}

	/* Use the driver API's to configure UART */
	ADI_UART_PARITY parity = ADI_UART_NO_PARITY;
	ADI_UART_STOPBITS stopbits = ADI_UART_ONE_STOPBIT;
	ADI_UART_WORDLEN  wordlen = ADI_UART_WORDLEN_8BITS;
	result = adi_uart_SetConfiguration(hDevice, parity, stopbits, wordlen);
	if (result != ADI_UART_SUCCESS){
		printf("UART SetConfiguration Error\n");
		while(1){ ; }
	}

	uint32_t BaudRate = 115200u;
	result = adi_uart_SetBaudRate(hDevice, BaudRate);
	if (result != ADI_UART_SUCCESS){
		printf("UART SetBaudRate Error\n");
		while(1){ ; }
	}

	//	printf("Actual BaudRate: %d\n", BaudRate);
	//
	//	result = adi_uart_GetBaudRate(hDevice, &BaudRate);
	//	if (result != ADI_UART_SUCCESS){
	//		printf("UART GetBaudRate Error\n");
	//		while(1){ ; }
	//	}

	/* submit the data to the UART device */
	result = adi_uart_SubmitTxBuffer(hDevice, buffer, sizeof(buffer));
	if (result != ADI_UART_SUCCESS){
		printf("UART Submit TX Buffer Error\n");
		while(1){ ; }
	}

	/* enable the UART transfer */
	result = adi_uart_EnableTx(hDevice, true);
	if (result != ADI_UART_SUCCESS){
		printf("UART EnableTx Error\n");
		while(1){ ; }
	}

	while (DEF_ON){

		ADI_GPIO_RESULT res_gpio = adi_gpio_Toggle(ADI_GPIO_PORT_F0, ADI_GPIO_PIN_6);
		if (res_gpio != ADI_GPIO_SUCCESS){
			printf("GPIO Toggle Error\n");
			while(1){ ; }
		}

		bool available = false;
		result = adi_uart_IsTxBufferAvailable(hDevice, &available);
		if (result != ADI_UART_SUCCESS){
			printf("UART IsBufferAvailable Error\n");
			while(1){ ; }
		}
		if (available){
			result = adi_uart_GetTxBuffer(hDevice, (void *)&pBuffer);
			if (result != ADI_UART_SUCCESS){
				printf("UART GetTxBuffer Error\n");
				while(1){ ; }
			}
			/* submit the data to the UART device */
			result = adi_uart_SubmitTxBuffer(hDevice, buffer, sizeof(buffer));
			if (result != ADI_UART_SUCCESS){
				printf("UART SubmitTxBuffer Error\n");
				while(1){ ; }
			}
		}// if (available)

		// Suspend UART Thread for 10 milliseconds
		CPU_INT32U mills = 10u;
		OSTimeDlyHMSM((CPU_INT16U)0,
				(CPU_INT16U)0,
				(CPU_INT16U)0,
				mills,
				(OS_OPT)OS_OPT_TIME_HMSM_STRICT,
				&err);
		if (err != OS_ERR_NONE) {
			printf("Error OSTimeDlyHMSM\n");
			while(1){ ; }
		}

//		OSTimeDly(1000, OS_OPT_TIME_PERIODIC, &err);
//		if (err != OS_ERR_NONE) {
//			printf("Error OSTimeDlyHMSM\n");
//			while(1){ ; }
//		}

	}// while {DEF_ON}

}






