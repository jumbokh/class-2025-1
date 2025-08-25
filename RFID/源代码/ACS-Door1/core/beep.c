#include "LPC11xx.h"     
#include "gpio.h"
/*************************Beep ¹¦ÄÜ************************************************/
#define CFG_BEEP_PORT				3
#define CFG_BEEP_PIN					3
#define CFG_BEEP_OFF                  (0)
#define CFG_BEEP_ON                  (1)

#define CFG_LED_ON                  (0)
#define CFG_LED_OFF                 (1)
void beepOn()
{
    gpioSetDir ( CFG_BEEP_PORT, CFG_BEEP_PIN, gpioDirection_Output );
    gpioSetValue ( CFG_BEEP_PORT, CFG_BEEP_PIN, CFG_BEEP_ON );
}


void beepOff()
{
    gpioSetDir ( CFG_BEEP_PORT, CFG_BEEP_PIN, gpioDirection_Output );
    gpioSetValue ( CFG_BEEP_PORT, CFG_BEEP_PIN, CFG_BEEP_OFF );
}
/*************************************************************************/

