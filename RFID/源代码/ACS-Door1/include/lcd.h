/****************************************************************************
*                                                                           *
* File:         lm3033.h                                                    *
*                                                                           *
* Version:      1.0                                       	       		    *
*                                                                           *
* Created:      06.06.2011                                                  *
* Last Change:  06.06.2011                                                  *
*                                                                           *
* Author:       Sui Libin                                                   *
*                                                                           *
* Compiler:     KEIL ARM                                                    *
*                                                                           *
* Description:  LPC1114-Firmware for LM3033(Serial Interface)               *
*                                                                           *
****************************************************************************/

/*---------------------------------------------------------------
-------------------------------------------------
LM3033: 1(VSS) | 2(VDD)| 3(V0) | 4(RS)  |5(SID)
MCU:      GND  |   5   |  NC   |  +5    | MOSI
-------------------------------------------------
LM3033: 6(SCLK)|7(DB0) |8(DB1) | 9(DB2) |10(DB3)
MCU:      SCK  |  NC   |  NC   |  NC    | NC
--------------------------------------------------
LM3033: 11(DB4)|12(DB5)|13(DB6)|14(DB7) |15(PSB)
MCU:      NC   |  NC   |  NC   |  NC    | GND
--------------------------------------------------
LM3033: 16(NC) |17(RST)|18(NC) |19(LEDA)|20(LEDK)
MCU:      NC   |  NC   |  NC   |  +5    | GND
---------------------------------------------------
---------------------------------------------------------------*/

#ifndef _LM3033_SERIAL_LCD_H_
#define _LM3033_SERIAL_LCD_H_


#define setbit(port,bit) (gpioSetValue(port, bit, 1))
#define clrbit(port,bit) (gpioSetValue(port, bit, 0))
#define tstbit(port,bit) ((gpioGetValue(port, bit))&1)

//lcd状态有关
#define LCD_BUSY_TIMES  10					//定义出现LCD忙的次数 
#define lcdBusy()		((getState()& 0x80)?1:0)
#define lcdChkBusyCnt()	{if(LCD_BusyCnt > LCD_BUSY_TIMES) {LCD_BusyCnt = 0; //重启动，添加应用程序} 
#define lcdClear()		{WriteCommand(0x01);delayms(10);}
#define lcdOpen()		WriteCommand(0x0c)
//文本显示有关
#define FunctionSet()	WriteCommand(0x30)  //功能设置 8位数据，基本指令 
#define DisplayOn()	WriteCommand(0x0c)  //显示状态 ON，游标OFF，反白OFF 
#define DisplayOff()	WriteCommand(0x08)  //显示状态 OFF，游标OFF，反白OFF 
#define EntryModeSet()	WriteCommand(0x06)
#define lcdStartLine(n) 	WriteCommand((8+(n))<<4)
#define RST_TO_WORK	 {clrbit(LCD_CTRL,RST);setbit(LCD_CTRL,RST);}
#define DisplayReverseWhite(line) {EX_FT();WriteCommand( 0x04 |((line)&0x03) ); FunctionSet();}
#define CursorDisp()  	WriteCommand(0x0e)
#define CursorFlash() 	WriteCommand(0x0d)   //光标闪烁 
#define CursorOff()   	WriteCommand(0x0c)   //关闭光标 
//绘图显示有关
#define EX_FT()		WriteCommand(0x36)
#define PicClear()		ClearArea(64,16,0,0,0)

#define CFG_LCD_CTRL_PORT 1
#define CFG_LCD_RS   5 		//CS   P1.5
#define CFG_LCD_RST 4		//RST   P1.4
#define CFG_LCD_DATA_PORT 3
#define CFG_LCD_SID	2	//SID  P3.2

#define CFG_LCD_SCLK_PORT    1 	//SCK  P1.1
#define CFG_LCD_SCLK    11 	//SCK  P1.11





extern uint8_t LCD_BusyCnt;
void WriteCommand ( uint8_t command );
void WriteData ( uint8_t data );

void LCD_ClearRam ( void );
void LCD_DisplayChinese ( unsigned char x, unsigned char y, const char *p );
void LCD_DisplayStrings ( unsigned char x, unsigned char y, const char *p );
void LCD_DisplayUINT32 ( unsigned char x, unsigned char y, unsigned long val );
void lcd_init(void);


//已经基本参数
#define LCD_X_MAX	127
#define LCD_Y_MAX	63



//每行的首地址,基于RT12864-4M型液晶
#define LINE_ONE_ADDRESS   0x80
#define LINE_TWO_ADDRESS   0x90
#define LINE_THREE_ADDRESS 0x88
#define LINE_FOUR_ADDRESS  0x98

//基本指令集预定义
#define LCD_DATA               	1         	//数据位
#define LCD_COMMAND            	0		 	//命令位
#define	LCD_CLEAR_SCREEN       	0x01 	 	//清屏	
#define	LCD_ADDRESS_RESET      	0x02		//地址归零	 
#define	LCD_BASIC_FUNCTION	  	0x30		//基本指令集
#define	LCD_EXTEND_FUNCTION	   	0x34		//扩充指令集

//扩展指令集预定义
#define	LCD_AWAIT_MODE			0x01	 	//待命模式	
#define LCD_ROLLADDRESS_ON		0x03		//允许输入垂直卷动地址
#define LCD_IRAMADDRESS_ON		0x02		//允许输入IRAM地址	
#define	LCD_SLEEP_MODE			0x08	 	//进入睡眠模式
#define	LCD_NO_SLEEP_MODE		0x0c	 	//脱离睡眠模式
#define LCD_GRAPH_ON			0x36		//打开绘图模式
#define LCD_GRAPH_OFF			0x34		//关闭绘图模式


#ifndef LCD_COLOR
#define LCD_COLOR_BLACK			1
#define LCD_COLOR_WHITE			0
#endif

#endif


