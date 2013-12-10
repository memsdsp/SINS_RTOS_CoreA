/*
 **************************************************************************
 *
 */

#ifndef _X_REPORT_
#define _X_REPORT_

#include "X_struct.h"
#include "X_parser.h"



#pragma pack(1)
typedef struct{
	float adc_channels[8];
    unsigned char check_sum;
} ADCPacketStructure;




#pragma pack()


unsigned char fill_buffer (PKT *rpt, unsigned char *buffer);
void pktADC(PKT *rpt, float * data);

#endif
