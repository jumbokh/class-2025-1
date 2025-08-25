#ifndef _STORM_CONFIG_H_ //条件定义
#define _STORM_CONFIG_H_

//#define UART_DEBUG_SHOW

#define DEFAULT_PURSE_BLOCK 56
#define DEFAULT_PURSE_DECR  160

#define CFG_CPU_CCLK 				 (48000000) //48MHz

#define CFG_BEEP_PORT				3 //MCU的3号端口
#define CFG_BEEP_PIN					3
#define CFG_BEEP_OFF                  (0) 
#define CFG_BEEP_ON                  (1)

#define CFG_LED_ON                  (0)
#define CFG_LED_OFF                 (1)

#define CFG_LED1_PORT				3
#define CFG_LED1_PIN					1

#define CFG_LED2_PORT				3
#define CFG_LED2_PIN					0

#define CFG_LED3_PORT				1
#define CFG_LED3_PIN					2
#define CFG_LED3_IOCON				IOCON_JTAG_nTRST_PIO1_2

#define CFG_LED4_PORT				1
#define CFG_LED4_PIN					1
#define CFG_LED4_IOCON				IOCON_JTAG_TDO_PIO1_1

#define CFG_LED5_PORT				2
#define CFG_LED5_PIN					8


#define CFG_SW1_PORT		0
#define CFG_SW1_PIN			7

#define CFG_SW2_PORT		0
#define CFG_SW2_PIN			6

#define CFG_SW3_PORT		3
#define CFG_SW3_PIN			5

#define CFG_SW4_PORT		2
#define CFG_SW4_PIN			5

#define CFG_LED_PORT				2
#define CFG_LED_PIN					7

#define CFG_SYSTICK_DELAY_IN_MS      (1)

#define CFG_URT
#define CFG_UART_BAUDRATE           (115200)
#define CFG_UART_BUFSIZE            (128)

#define CFG_INTERFACE_MAXMSGSIZE    (256)
#define CFG_INTERFACE_PROMPT        "Storm >> "
#define CFG_PRINTF_NEWLINE          "\r\n"

#define CFG_TIMER32

/* 2.4G IO definitions */
#define VCCN_2G4_PORT  2
#define VCCN_2G4_PIN   9
#define VCCN_2G4_IOCON  	IOCON_PIO2_9

#define CE_2G4_PORT  1
#define CE_2G4_PIN   10
#define CE_2G4_IOCON  	IOCON_PIO1_10

#define CSN_2G4_PORT   0
#define CSN_2G4_PIN    2
#define CSN_2G4_IOCON  	IOCON_PIO0_2

#define IRQ_2G4_PORT   0
#define IRQ_2G4_PIN    11
#define IRQ_2G4_IOCON  	IOCON_JTAG_TDI_PIO0_11

#define SCK_2G4_PORT   2
#define SCK_2G4_PIN    11
#define SCK_2G4_IOCON  	IOCON_PIO2_11

#define MISO_2G4_PORT   0
#define MISO_2G4_PIN    8
#define MISO_2G4_IOCON  	IOCON_PIO0_8

#define MOSI_2G4_PORT   0
#define MOSI_2G4_PIN    9
#define MOSI_2G4_IOCON  	IOCON_PIO0_9


/* 125k IO definitions */
#define VCCN_125K_PORT 	2
#define VCCN_125K_PIN  	6
#define VCCN_125K_IOCON  	IOCON_PIO2_6

#define WG1_125K_PORT 2
#define WG1_125K_PIN  4
#define WG1_125K_IOCON IOCON_PIO2_4

#define WG2_125K_PORT 1
#define WG2_125K_PIN  8
#define WG2_125K_IOCON IOCON_PIO1_8

#define ACT_FLAG_125K_PORT  3
#define ACT_FLAG_125K_PIN   4
#define ACT_FLAG_125K_IOCON IOCON_PIO3_4


/* 13.56M IO definitions */
#define VCCN_13M56_PORT 0
#define VCCN_13M56_PIN  3
#define VCCN_13M56_IOCON  IOCON_PIO0_3


#define SCL_13M56_PORT  0
#define SCL_13M56_PIN   4
#define SCL_13M56_IOCON  IOCON_PIO0_4


#define SDA_13M56_PORT   0
#define SDA_13M56_PIN    5
#define SDA_13M56_IOCON  IOCON_PIO0_5

#define WAKE_13M56_PORT   1
#define WAKE_13M56_PIN    9
#define WAKE_13M56_IOCON  IOCON_PIO1_9

/* STN IO definitions */
#define CS_STN_PORT    1
#define CS_STN_PIN	   5

#define SID_STN_PORT    3
#define SID_STN_PIN     2

#define SCK_STN_PORT   1
#define SCK_STN_PIN    1

#define RSTn_STN_PORT  1
#define RSTn_STN_PIN   4

/*继电器*/
#define RELAY_OFF CFG_LED_OFF
#define RELAY_ON   CFG_LED_ON
#define RELAY_PORT  2
#define RELAY_PIN   10
#define RELAY_IOCON  IOCON_PIO2_10

#endif

