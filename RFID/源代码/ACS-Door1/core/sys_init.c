#include "LPC11xx.h"     
#include "gpio.h"
#include "config.h"


void other_gpio_config (void)
{
    CFG_LED3_IOCON &= ~IOCON_JTAG_nTRST_PIO1_2_FUNC_MASK;
    CFG_LED3_IOCON |= IOCON_JTAG_nTRST_PIO1_2_FUNC_GPIO;
	
    CFG_LED4_IOCON &= ~IOCON_JTAG_TDO_PIO1_1_FUNC_MASK;
    CFG_LED4_IOCON |= IOCON_JTAG_TDO_PIO1_1_FUNC_GPIO;

//2.4G Module POWER OFF
    VCCN_2G4_IOCON &= ~IOCON_PIO2_9_FUNC_MASK;
    VCCN_2G4_IOCON |= IOCON_PIO2_9_FUNC_GPIO;
    gpioSetDir ( VCCN_2G4_PORT, VCCN_2G4_PIN, gpioDirection_Output );
    gpioSetValue ( VCCN_2G4_PORT, VCCN_2G4_PIN, 1 );

//125KRFID Module POWER OFF    
	VCCN_125K_IOCON &= ~IOCON_PIO2_6_FUNC_MASK;
    VCCN_125K_IOCON |= IOCON_PIO2_6_FUNC_GPIO;
    gpioSetDir ( VCCN_125K_PORT, VCCN_125K_PIN, gpioDirection_Output );
    gpioSetValue ( VCCN_125K_PORT, VCCN_125K_PIN, 1 );

//13M56RFID Module POWER OFF 
	VCCN_13M56_IOCON &= ~IOCON_PIO0_3_FUNC_MASK;
    VCCN_13M56_IOCON |= IOCON_PIO0_3_FUNC_GPIO;
    gpioSetDir ( VCCN_13M56_PORT, VCCN_13M56_PIN, gpioDirection_Output );
    gpioSetValue ( VCCN_13M56_PORT, VCCN_13M56_PIN, 1 );

}


/***********************系统时钟初始化函数*******************************************/

void cpuPllSetup ( int multiplier )
{
    uint32_t i;

    // Power up system oscillator
    SCB_PDRUNCFG &= ~ ( SCB_PDRUNCFG_SYSOSC_MASK );

    // Setup the crystal input (bypass disabled, 1-20MHz crystal)
    SCB_SYSOSCCTRL = ( SCB_SYSOSCCTRL_BYPASS_DISABLED | SCB_SYSOSCCTRL_FREQRANGE_1TO20MHZ );

    for ( i = 0; i < 200; i++ )
    {
        __NOP();
    }

    // Configure PLL
    SCB_PLLCLKSEL = SCB_CLKSEL_SOURCE_MAINOSC;    // Use the external crystal
    SCB_PLLCLKUEN = SCB_PLLCLKUEN_UPDATE;         // Update clock source
    SCB_PLLCLKUEN = SCB_PLLCLKUEN_DISABLE;        // Toggle update register once
    SCB_PLLCLKUEN = SCB_PLLCLKUEN_UPDATE;         // Update clock source again

    // Wait until the clock is updated
    while ( ! ( SCB_PLLCLKUEN & SCB_PLLCLKUEN_UPDATE ) );


    // Set clock speed
    switch ( multiplier )
    {
    case 0:
        SCB_PLLCTRL = ( SCB_PLLCTRL_MULT_2 | ( 1 << SCB_PLLCTRL_DIV_BIT ) );
        break;
    case 1:
        SCB_PLLCTRL = ( SCB_PLLCTRL_MULT_3 | ( 1 << SCB_PLLCTRL_DIV_BIT ) );
        break;
    case 2:
        SCB_PLLCTRL = ( SCB_PLLCTRL_MULT_4 | ( 1 << SCB_PLLCTRL_DIV_BIT ) );
        break;
    case 3:
    default:
        SCB_PLLCTRL = ( SCB_PLLCTRL_MULT_1 | ( 1 << SCB_PLLCTRL_DIV_BIT ) );
        break;
    }


    // Enable system PLL
    SCB_PDRUNCFG &= ~ ( SCB_PDRUNCFG_SYSPLL_MASK );

    // Wait for PLL to lock
    while ( ! ( SCB_PLLSTAT & SCB_PLLSTAT_LOCK ) );

    // Setup main clock
    SCB_MAINCLKSEL = SCB_MAINCLKSEL_SOURCE_SYSPLLCLKOUT;
    SCB_MAINCLKUEN = SCB_MAINCLKUEN_UPDATE;       // Update clock source
    SCB_MAINCLKUEN = SCB_MAINCLKUEN_DISABLE;      // Toggle update register once
    SCB_MAINCLKUEN = SCB_MAINCLKUEN_UPDATE;

    // Wait until the clock is updated
    while ( ! ( SCB_MAINCLKUEN & SCB_MAINCLKUEN_UPDATE ) );

    // Set system AHB clock
    SCB_SYSAHBCLKDIV = SCB_SYSAHBCLKDIV_DIV1;

    // Enabled IOCON clock for I/O related peripherals
    SCB_SYSAHBCLKCTRL |= SCB_SYSAHBCLKCTRL_IOCON;
}
void systemInit()
{

    gpioInit(); /*初始化gpio模式时钟选择*/

	other_gpio_config();

    cpuPllSetup(3);/*设置系统时钟12M*/

    gpioSetDir ( 0, 7, gpioDirection_Input); /*设置管脚0.7为输入模式*/

}

/*************************************************************************/

