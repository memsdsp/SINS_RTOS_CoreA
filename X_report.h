/*
 **************************************************************************
 *
 */

#ifndef _X_REPORT_
#define _X_REPORT_

#include <ccblkfn.h>
#include <stdint.h>
#include "X_struct.h"
#include "X_parser.h"
#include "SINS_RTOS_CoreA.h"

#pragma pack(1)
typedef struct{
	float adc_channels[8];
    unsigned char check_sum;
} ADCPacketStructure;

typedef struct{
	uint32_t   fcclk;
	uint32_t   fsclk;
	uint32_t BaudRate;
    unsigned char check_sum;
} AlivePacketStructure;


#pragma pack()


unsigned char fill_buffer (PKT *rpt, unsigned char *buffer);
void pktADC(PKT *rpt, float * data);
void pktAlive(PKT *rpt, SystemParameters *data);

#endif
