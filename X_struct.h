#ifndef _X_STRUCT_
#define _X_STRUCT_
/*
 **************************************************************************
 *
 */

#ifndef FALSE
#define FALSE 	(0)
#define TRUE 	(!FALSE)
#endif

/* control characters for TSIP packets */
#define DLE 	(0x10)
#define ETX 	(0x03)

#define MAX_BUF (256)

/* values of TSIPPKT.status */
#define PARSED_EMPTY 	0
#define PARSED_FULL 	1
#define PARSED_DLE_1 	2
#define PARSED_DATA 	3
#define PARSED_DLE_2 	4

/* packets have the following structure, whether report or command. */
typedef struct {
	short counter;			/* counter */
    short len;				/* size of buf; < MAX_BUF unsigned chars */
	unsigned char
		status,		   	    /* packet format/parse status */
		code;				/*  code */
	unsigned char
		buf[MAX_BUF];	    /* report or command string */
} PKT;
#endif

