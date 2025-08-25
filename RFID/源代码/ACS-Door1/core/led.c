#include "LPC11xx.h"     
#include "gpio.h"
#include "config.h"
/*************************LED 功能************************************************/

//BAK2 pin on mainboard	
void Bak2On(void)
{
     gpioSetDir ( CFG_LED_PORT, CFG_LED_PIN, gpioDirection_Output );
	gpioSetValue ( CFG_LED_PORT, CFG_LED_PIN, CFG_LED_ON );
}

void Bak2Off(void)
{
    gpioSetDir ( CFG_LED_PORT, CFG_LED_PIN, gpioDirection_Output );
    gpioSetValue ( CFG_LED_PORT, CFG_LED_PIN, CFG_LED_OFF );
}


//LED1 indicator
void Led1On(void)
{

    gpioSetDir ( CFG_LED1_PORT, CFG_LED1_PIN, gpioDirection_Output );
    gpioSetValue ( CFG_LED1_PORT, CFG_LED1_PIN, CFG_LED_ON );
}

void Led1Off(void)
{

    gpioSetDir ( CFG_LED1_PORT, CFG_LED1_PIN, gpioDirection_Output );
    gpioSetValue ( CFG_LED1_PORT, CFG_LED1_PIN, CFG_LED_OFF );
}	


//LED2 indicator
void Led2On(void)
{
	
    gpioSetDir ( CFG_LED2_PORT, CFG_LED2_PIN, gpioDirection_Output );//gpio设置成输出
    gpioSetValue ( CFG_LED2_PORT, CFG_LED2_PIN, CFG_LED_ON );	   //gpio值设置 “1”LED OFF,"0"LED ON
}	

void Led2Off(void)
{
	
    gpioSetDir ( CFG_LED2_PORT, CFG_LED2_PIN, gpioDirection_Output );
    gpioSetValue ( CFG_LED2_PORT, CFG_LED2_PIN, CFG_LED_OFF );	   //gpio值设置 “1”LED OFF,"0"LED ON
}	


//LED3 indicator
void Led3On(void)
{
	
    gpioSetDir ( CFG_LED3_PORT, CFG_LED3_PIN, gpioDirection_Output );
    gpioSetValue ( CFG_LED3_PORT, CFG_LED3_PIN, CFG_LED_ON );
}	

void Led3Off(void)
{
	
    gpioSetDir ( CFG_LED3_PORT, CFG_LED3_PIN, gpioDirection_Output );
    gpioSetValue ( CFG_LED3_PORT, CFG_LED3_PIN, CFG_LED_OFF );
}	


//LED4 indicator
void Led4On(void)
{
	
    gpioSetDir ( CFG_LED4_PORT, CFG_LED4_PIN, gpioDirection_Output );
    gpioSetValue ( CFG_LED4_PORT, CFG_LED4_PIN, CFG_LED_ON );
}	

void Led4Off(void)
{
	
    gpioSetDir ( CFG_LED4_PORT, CFG_LED4_PIN, gpioDirection_Output );
    gpioSetValue ( CFG_LED4_PORT, CFG_LED4_PIN, CFG_LED_OFF );
}	


//LED5 indicator
void Led5On(void)
{
	
    gpioSetDir ( CFG_LED5_PORT, CFG_LED5_PIN, gpioDirection_Output );
    gpioSetValue ( CFG_LED5_PORT, CFG_LED5_PIN, CFG_LED_ON );
}	

void Led5Off(void)
{
	
    gpioSetDir ( CFG_LED5_PORT, CFG_LED5_PIN, gpioDirection_Output );
    gpioSetValue ( CFG_LED5_PORT, CFG_LED5_PIN, CFG_LED_OFF );
}	



	
/*************************************************************************/


