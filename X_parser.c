/*
 *
 * *************************************************************************
 *
 */

#include <string.h>
#include "X_struct.h"
#include "X_parser.h"

/**/
void parce_uart_input (PKT *rpt, unsigned char newbyte)
/* reads bytes until serial buffer is empty or a complete report
 * has been received; end of report is signified by DLE ETX.
 */
{
	switch (rpt->status)
	{
	case PARSED_DLE_1:
		switch (newbyte)
		{
		case 0:
		case ETX:
      	/* illegal  IDs */
         rpt->len = 0;
			rpt->status = PARSED_EMPTY;
			break;
		case DLE:
      	/* try normal message start again */
			rpt->len = 0;
			rpt->status = PARSED_DLE_1;
			break;
		default:
      	/* legal  ID; start message */
			rpt->code = newbyte;
         rpt->len = 0;
			rpt->status = PARSED_DATA;
			break;
		}
		break;
	case PARSED_DATA:
		switch (newbyte) {
		case DLE:
      	/* expect DLE or ETX next */
			rpt->status = PARSED_DLE_2;
			break;
		default:
      	/* normal data byte  */
			rpt->buf[rpt->len] = newbyte;
			rpt->len++;
         /* no change in rpt->status */
			break;
		}
		break;
	case PARSED_DLE_2:
		switch (newbyte) {
		case DLE:
      	/* normal data byte */
			rpt->buf[rpt->len] = newbyte;
			rpt->len++;
			rpt->status = PARSED_DATA;
			break;
		case ETX:
			/* end of message; return TRUE here. */
			rpt->status = PARSED_FULL;
			break;
		default:
			/* error: treat as PARSED_DLE_1; start new report packet */
			rpt->code = newbyte;
         rpt->len = 0;
			rpt->status = PARSED_DATA;
		}
		break;
	case PARSED_FULL:
	case PARSED_EMPTY:
	default:
		switch (newbyte) {
		case DLE:
      	/* normal message start */
			rpt->len = 0;
			rpt->status = PARSED_DLE_1;
			break;
		default:
			/* error: ignore newbyte */
			rpt->len = 0;
			rpt->status = PARSED_EMPTY;
		}
		break;
	}
	if (rpt->len > MAX_BUF) {
		/* error: start new report packet */
		rpt->status = PARSED_EMPTY;
		rpt->len = 0;
	}
}


/**/
char cmd_0x3D (PKT *rpt, unsigned char *command)
{

	if (rpt->len != 1) return FALSE;
	*command = rpt->buf[0];
	return TRUE;
}


