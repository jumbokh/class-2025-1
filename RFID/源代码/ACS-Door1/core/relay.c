#include "LPC11xx.h"     
#include "gpio.h"
#include "config.h"
/*************************Relay ¹¦ÄÜ************************************************/


void RelayOn(void)
{
    
    gpioSetDir ( RELAY_PORT, RELAY_PIN, gpioDirection_Output );
    gpioSetValue ( RELAY_PORT, RELAY_PIN, CFG_LED_ON );
}


void RelayOff(void)
{
    
    gpioSetDir ( RELAY_PORT, RELAY_PIN, gpioDirection_Output );
    gpioSetValue ( RELAY_PORT, RELAY_PIN, CFG_LED_OFF );
}
/*************************************************************************/


