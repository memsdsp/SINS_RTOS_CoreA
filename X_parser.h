#ifndef _X_PARSER_
#define _X_PARSER_

#include "X_struct.h"

#define CMD_START    0x25
#define CMD_STOP     0x27
#define CMD_RESET    0x29
#define CMD_CALIBR   0xAB
#define CMD_QUERY    0xAD
#define CMD_CFS_ERASE    0xAE

/*
 *
 **************************************************************************
 *
 */

/*******************        PROTOYPES            *****************/

/* functions to add a byte from the receiver and "unstuffs" it */
void parce_uart_input (PKT *rpt, unsigned char newbyte);

/**/
/*  prototypes for report parsing primitives */
char cmd_0x3D (PKT *rpt, unsigned char *command);

#endif

