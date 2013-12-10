#include <string.h>
#include <stdio.h>
#include "X_report.h"


/**/
unsigned char fill_buffer (PKT *rpt, unsigned char *buffer)

{
	unsigned char k = 0, i = 0;

	buffer[k++] = DLE;
	buffer[k++] = rpt->code;
	for (; i < (rpt->len); i++){
		if (rpt->buf[i] == DLE){
			buffer[k++] = DLE;
			buffer[k++] = DLE;
		} else {
			buffer[k++] = rpt->buf[i];
		}
	}
	buffer[k++] = DLE;
	buffer[k++] = ETX;
	
	return k;
}

// DATA PACKET FOR IMU RESULTS 
void pktADC(PKT *rpt, float * data)
{
	ADCPacketStructure pack;
	unsigned char check_sum = 0;
	unsigned char *cbuf, *cbufend;

	memcpy(pack.adc_channels, data, sizeof(float)*8);
	
	cbufend = (unsigned char *)&pack+sizeof(ADCPacketStructure)-1;
	for (cbuf = (unsigned char *)&pack; cbuf < cbufend; cbuf++)
		check_sum ^= *cbuf;
	pack.check_sum = check_sum;
	memcpy(rpt->buf, &pack, sizeof(ADCPacketStructure));
	rpt->len = sizeof(ADCPacketStructure);
	rpt->code = 0xAA;
}



