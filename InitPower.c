/*
 * InitPower.c
 *
 *  Created on: 04.12.2013
 *      Author: small
 */

#include <services/pwr/adi_pwr.h>
#include <stdio.h>
#define MHZTOHZ       (1000000)

#define CLKIN         (25  * MHZTOHZ)
#define CORE_MAX      (500 * MHZTOHZ)
#define SYS_MAX       (125 * MHZTOHZ)
#define VCO_MIN       (50  * MHZTOHZ)

void InitPower(void)
{
	ADI_PWR_RESULT result;

	/* initialize the power service with the clock limit values */
	result = adi_pwr_Init(CLKIN, CORE_MAX, SYS_MAX, VCO_MIN);
	if (result != ADI_PWR_SUCCESS){
		printf("adi_pwr_Init fail!\n");
	}

	/* set the core clock and system clock values */
	result = adi_pwr_SetFreq(500 * MHZTOHZ, 125 * MHZTOHZ);
	if (result != ADI_PWR_SUCCESS){
		printf("adi_pwr_SetFreq fail!\n");
	}

//	/* get the core clock */
//	uint32_t fcclk;
//	result = adi_pwr_GetCoreFreq(&fcclk);
//	if (result != ADI_PWR_SUCCESS){
//		printf("adi_pwr_GetCoreFreq fail!\n");
//	} else {
//		printf("core clock: %d\n",fcclk);
//	}

//	/* get the system clock*/
//	uint32_t fsclk;
//	result = adi_pwr_GetSystemFreq(&fsclk);
//	if (result != ADI_PWR_SUCCESS){
//		printf("adi_pwr_GetSystemFreq fail!\n");
//	} else {
//		printf("system clock: %d\n",fsclk);
//	}

}


