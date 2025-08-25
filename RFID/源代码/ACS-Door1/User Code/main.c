#include "LPC11xx.h"     
#include "gpio.h"
#include "lcd.h"

extern void delayms (uint32_t ulTime);	//外部函数申明
extern void  systemInit(void); 

extern void beepOff(void);

int main (void)
{
	//unsigned char i = 120;
    
	systemInit(); //系统初始化（必不可少）                                                      /* 系统初始化，切勿删除         */

	beepOff ();	 //关闭蜂鸣器

	lcd_init();	   //LCD初始化 （必不可少）
					 
    LCD_ClearRam();	//清屏


	
	LCD_DisplayChinese ( 0, 0,  "欢迎使用门禁系统" );
	LCD_DisplayChinese ( 0, 1,  "请刷卡  " )	;


//	delayms(200);  //加延时
	rfid125k_read2 (); 

    }


