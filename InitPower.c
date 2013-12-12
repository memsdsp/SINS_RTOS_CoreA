/*
 * InitPower.c
 *
 *  Created on: Oct 16, 2013
 *      Author: user
 */

#include <services\pwr\adi_pwr.h>
#include <ccblkfn.h>
#include <stdio.h>
#include "InitPower.h"


static uint32_t callbackEvents;
static bool bError;

static void CheckResult(ADI_PWR_RESULT result)
{
	if (result != ADI_PWR_SUCCESS) {
		printf("Power service failure\n");
		bError = true;
	}
}

/* Reset target to default power */
static void setDefaultPower(void)
{
	ADI_PWR_RESULT result;

	result = adi_pwr_SetControlRegister(ADI_PWR_CTL_DF, DF_DEFAULT);
	result = adi_pwr_SetControlRegister(ADI_PWR_CTL_MSEL, MSEL_DEFAULT);
	result = adi_pwr_SetControlRegister(ADI_PWR_CTL_PLL_OFF, 0);
	result = adi_pwr_SetControlRegister(ADI_PWR_CTL_BYPASS, 0);
	result = adi_pwr_SetDivideRegister(ADI_PWR_DIV_SSEL, SSEL_DEFAULT);
	result = adi_pwr_SetDivideRegister(ADI_PWR_DIV_CSEL, CSEL_DEFAULT);
}

/* Callback function  */
static void PWRCallback(void *pCBParam, uint32_t Event, void *pArg)
{
	uint32_t sclk;
	ADI_PWR_MODE  mode;

	switch ( (ADI_PWR_EVENT)Event )
	{
    case ADI_PWR_EVENT_FREQ_PRE_CHANGE:
    	sclk = *(uint32_t*)(pArg);
    	callbackEvents++;
    	break;
    case ADI_PWR_EVENT_FREQ_POST_CHANGE:
    	sclk = *(uint32_t*)(pArg);
    	callbackEvents++;
    	break;
    case ADI_PWR_EVENT_MODE_PRE_CHANGE:
    	mode = *(ADI_PWR_MODE*)pArg;
    	callbackEvents++;
    	break;
    case ADI_PWR_EVENT_MODE_POST_CHANGE:
    	mode = *(ADI_PWR_MODE*)pArg;
    	callbackEvents++;
    	break;
	}
}

void InitPower(void)
{
	uint32_t   fcclk = 0;
	uint32_t   fsclk = 0;
	uint32_t   fvco = 0;
	uint32_t   core;
	uint32_t   system;
	uint32_t expectedEvents;
	ADI_PWR_RESULT result;



	bError = false;
	callbackEvents = 0;
	expectedEvents = 0;

	result = adi_pwr_Init(CLKIN, CORE_MAX, SYSTEM_MAX, VCO_MIN);
	CheckResult(result);

	result = adi_pwr_InstallCallback(PWRCallback);
	CheckResult(result);

	setDefaultPower();

	// set max freq
	result = adi_pwr_SetFreq(CORE_MAX, SYSTEM_MAX);
	CheckResult(result);

	expectedEvents += 2; //pre and post event

	// get the freq settings
	result = adi_pwr_GetCoreFreq(&fcclk);
	CheckResult(result);

	result = adi_pwr_GetSystemFreq(&fsclk);
	CheckResult(result);
	if ((fcclk < (CORE_MAX-CLKIN)) || (fsclk < (SYSTEM_MAX-CLKIN)))
	{
		bError = true;
	}

}


