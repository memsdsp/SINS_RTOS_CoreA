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
#include "X_report.h"
#include "SINS_RTOS_CoreA.h"

#define UART_DEVICE_NUM 0u
#define BAUD_RATE 115200u
#define MILLS 10u
/*-------------------Externals------------------------*/
extern OS_MUTEX   MutexUARTSend;
extern float adc_decimated_channels[8];
extern SystemParameters system_parameters;

/* UART driver memory */
uint8_t driverMemory[ADI_UART_UNIDIR_DMA_MEMORY_SIZE];

static PKT uart_packet;
static uint8_t uart_tx_buffer[128];
static uint8_t uart_rx_buffer[16];

void AppUARTThread(void* arg)
{

	OS_ERR err;

	/* Pointer to the buffer */
	uint8_t *pBuffer;

	/* buffer which holds data to transfer over UART */
	uint8_t buffer[] = {'S','I','N','S',' ','M','I','E','T','\n'};

	/* UART Driver Handle */
	ADI_UART_HANDLE hDevice;

	/* UART result code */
	ADI_UART_RESULT result;


	/* open the UART driver in Bidirectional mode */
	result = adi_uart_Open(UART_DEVICE_NUM, ADI_UART_DIR_BIDIRECTION, driverMemory,
			ADI_UART_BIDIR_DMA_MEMORY_SIZE, &hDevice);
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
	ADI_UART_PARITY parity = ADI_UART_EVEN_PARITY;
	ADI_UART_STOPBITS stopbits = ADI_UART_ONE_STOPBIT;
	ADI_UART_WORDLEN  wordlen = ADI_UART_WORDLEN_8BITS;
	result = adi_uart_SetConfiguration(hDevice, parity, stopbits, wordlen);
	if (result != ADI_UART_SUCCESS){
		printf("UART SetConfiguration Error\n");
		while(1){ ; }
	}

	uint32_t BaudRate = BAUD_RATE;
	result = adi_uart_SetBaudRate(hDevice, BaudRate);
	if (result != ADI_UART_SUCCESS){
		printf("UART SetBaudRate Error\n");
		while(1){ ; }
	}

	result = adi_uart_GetBaudRate(hDevice, &BaudRate);
	if (result != ADI_UART_SUCCESS){
		printf("UART SetBaudRate Error\n");
		while(1){ ; }
	}
	system_parameters.BaudRate = BaudRate;

	/* submit the TX Buffer to the UART device */
	result = adi_uart_SubmitTxBuffer(hDevice, buffer, sizeof(buffer));
	if (result != ADI_UART_SUCCESS){
		printf("UART Submit TX Buffer Error\n");
		while(1){ ; }
	}

	/* submit the RX Buffer to the UART device */
	result = adi_uart_SubmitRxBuffer(hDevice, uart_rx_buffer, sizeof(uart_rx_buffer));
	if (result != ADI_UART_SUCCESS){
		printf("UART Submit RX Buffer Error\n");
		while(1){ ; }
	}


	/* enable the UART Transmit */
	result = adi_uart_EnableTx(hDevice, true);
	if (result != ADI_UART_SUCCESS){
		printf("UART EnableTx Error\n");
		while(1){ ; }
	}

	/* enable the UART Receive */
	result = adi_uart_EnableRx(hDevice, true);
	if (result != ADI_UART_SUCCESS){
		printf("UART EnableRx Error\n");
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

			/*Check if there is Alive message*/
			OS_MSG_SIZE message_size;
			void *message_alive;
			message_alive = OSTaskQPend((OS_TICK)0u, OS_OPT_PEND_NON_BLOCKING, &message_size, NULL, &err);
			if (err == OS_ERR_NONE){
				//Send Alive Packet
				//fill alive packet structure
				pktAlive(&uart_packet, (SystemParameters *)message_alive);
			} else {
				//Send ADC Data Packet
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
				//copy decimated adc channels data to uart packet structure
				pktADC(&uart_packet, adc_decimated_channels);
				OSMutexPost((OS_MUTEX  *)&MutexUARTSend,
						(OS_OPT     )OS_OPT_POST_NONE,
						(OS_ERR    *)&err);
				if (err != OS_ERR_NONE) {
					printf("Error Posting Mutex\n");
					while(1){ ; }
				}
			}

			/* Fill UART transmit buffer */
			uint8_t len = fill_buffer(&uart_packet, uart_tx_buffer);

			/* submit the data to the UART device */
			result = adi_uart_SubmitTxBuffer(hDevice, uart_tx_buffer, len);
			if (result != ADI_UART_SUCCESS){
				printf("UART SubmitTxBuffer Error\n");
				while(1){ ; }
			}
		}// if (available)

		/* Check if there is filled Receive buffer */
		result = adi_uart_IsRxBufferAvailable(hDevice, &available);
		if (result != ADI_UART_SUCCESS){
			printf("UART IsBufferAvailable Error\n");
			while(1){ ; }
		}
		if (available){
			/* submit the RX Buffer to the UART device */
			result = adi_uart_SubmitRxBuffer(hDevice, uart_rx_buffer, sizeof(uart_rx_buffer));
			if (result != ADI_UART_SUCCESS){
				printf("UART Submit RX Buffer Error\n");
				while(1){ ; }
			}
		}

		// Suspend UART Thread for mills number of  millisecond
		CPU_INT32U mills = MILLS;
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

	}// while {DEF_ON}

}






