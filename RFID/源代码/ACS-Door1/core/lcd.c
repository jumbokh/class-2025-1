/****************************************************************************
*                                                                           *
* File:         lcd.c                                                    *
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

#include "LPC11xx.h"
#include "lcd.h"
#include "gpio.h"

#define CFG_CPU_CCLK 				 (48000000) //48MHz
void Delayus ( uint32_t x )
{
    uint32_t delay = x * ( ( CFG_CPU_CCLK / 100000 ) / 120 ); // Debug Mode (No optimisations)

    while ( delay > 0 )
    {
        __NOP();
        __NOP();
        __NOP();
        __NOP();

        delay--;
    }
}

void delayms ( uint32_t ms );
void BUSYLOOP ( void );

/**********************延时程序START********************************/

void BUSYLOOP ( void )
{
    Delayus ( 10 );
}
/**********************延时程序END********************************/


void out_data ( uint8_t val )
{
    char i, d;
    //setbit ( CFG_LCD_CTRL_PORT, CFG_LCD_RS );

    for ( i = 0; i < 8; i++ )
    {

        d = val & 0x80;
        if ( d ) 	 								//当前串行口发出的数据bit
        {
            setbit ( CFG_LCD_DATA_PORT, CFG_LCD_SID );
        }
        else
        {
            clrbit ( CFG_LCD_DATA_PORT, CFG_LCD_SID );
        }
        val <<= 1;

        clrbit ( CFG_LCD_SCLK_PORT, CFG_LCD_SCLK ); 	//产生SCLK下降沿

        Delayus ( 1 );
        setbit ( CFG_LCD_SCLK_PORT, CFG_LCD_SCLK ); 	//产生SCLK上升沿，发送数据
        Delayus ( 1 );
    }

}

void WriteCommand ( uint8_t command_data )
{
    uint8_t i_data, temp_data1, temp_data2;
    i_data = 0xf8;

    clrbit ( CFG_LCD_SCLK_PORT, CFG_LCD_SCLK );

    setbit ( CFG_LCD_CTRL_PORT, CFG_LCD_RS );

    out_data ( i_data );

    i_data = command_data;
    i_data &= 0xf0;
    out_data ( i_data );

    i_data = command_data;
    temp_data1 = i_data & 0xf0;
    temp_data2 = i_data & 0x0f;
    temp_data1 >>= 4;
    temp_data2 <<= 4;
    i_data = temp_data1 | temp_data2;
    i_data &= 0xf0;
    out_data ( i_data );
    clrbit ( CFG_LCD_SCLK_PORT, CFG_LCD_SCLK );

    clrbit ( CFG_LCD_CTRL_PORT, CFG_LCD_RS );

}


void WriteData ( uint8_t command_data )
{
    uint8_t i_data, temp_data1, temp_data2;

    i_data = 0xfa;
    //delayms ( 500 );
    clrbit ( CFG_LCD_SCLK_PORT, CFG_LCD_SCLK );

    setbit ( CFG_LCD_CTRL_PORT, CFG_LCD_RS );
    out_data ( i_data );

    i_data = command_data;
    i_data &= 0xf0;
    out_data ( i_data );

    i_data = command_data;
    temp_data1 = i_data & 0xf0;
    temp_data2 = i_data & 0x0f;
    temp_data1 >>= 4;
    temp_data2 <<= 4;
    i_data = temp_data1 | temp_data2;
    i_data &= 0xf0;
    out_data ( i_data );

    clrbit ( CFG_LCD_SCLK_PORT, CFG_LCD_SCLK );

    clrbit ( CFG_LCD_CTRL_PORT, CFG_LCD_RS );

}

void initLCDModule ( void );
void lcd_init()
{
    uint8_t command_data;
    initLCDModule();


    command_data = 0x30;
    WriteCommand ( command_data ); /*功能设置:一次送8位数据,基本指令集*/
    Delayus ( 100 );
    command_data = 0x01;
    WriteCommand ( command_data ); /*清DDRAM*/

    delayms ( 100 );

    command_data = 0x06;
    WriteCommand ( command_data ); /*点设定:显示字符/光标从左到右移位,DDRAM地址加1*/
    command_data = 0x0C;
    WriteCommand ( command_data ); /*显示设定:开显示,显示光标,当前显示位反白闪动*/

    command_data = 0x80;
    WriteCommand ( command_data ); /*把显示地址设为0X80，即为第一行的首位*/

}

uint8_t  tab31[] =
{
    "本系列中文模块内"
    "任意位置反白显示"
    "置二级字库，可在"
    "使用更方便更灵活"
};

uint8_t  tab32[] =
{
//"金鹏科技有限公司"
    "Golden Palm TEC1"
    "Golden Palm TEC2"
    "Golden Palm TEC3"
    "Golden Palm TEC4"
};
void display_cpubbs()
{
    uint8_t command_data;
    int i = 0;
    int j = 0;
    int loop = 20;
    while ( 1 )
    {
        command_data = 0x80;
        WriteCommand ( command_data ); /*DDRAM地址归位*/

        // for ( j = 0; j < 4; j++ )
        j = 0;
        {
            for ( i = 0; i < 16; i++ )
            {
                command_data = tab31[j * 16 + i];
                WriteData ( command_data );

            }
        }

        loop--;

        command_data = 0x90;
        WriteCommand ( command_data );

        command_data = 'A';
        WriteData ( command_data );


        command_data = 0x98;
        WriteCommand ( command_data );

        command_data = tab31[0];
        WriteData ( command_data );
        command_data = tab31[1];
        WriteData ( command_data );


        command_data = 0x88;
        WriteCommand ( command_data );

        command_data = tab31[2];
        WriteData ( command_data );
        command_data = tab31[3];
        WriteData ( command_data );
    }
}
/**********************串行方式相关程序END*********************/


/**********************液晶文字显示相关程序START*************/

void initLCDModule()
{
    IOCON_PIO1_5 &= ~IOCON_PIO1_5_FUNC_MASK;
    IOCON_PIO1_5 |= IOCON_PIO1_5_FUNC_GPIO;
    gpioSetDir ( CFG_LCD_CTRL_PORT, CFG_LCD_RS, gpioDirection_Output );
    gpioSetPullup ( &IOCON_PIO1_5, gpioPullupMode_PullUp );



    IOCON_PIO1_4 &= ~IOCON_PIO1_4_FUNC_MASK;
    IOCON_PIO1_4 |= IOCON_PIO1_4_FUNC_GPIO;
    gpioSetDir ( CFG_LCD_CTRL_PORT, CFG_LCD_RST, gpioDirection_Output );
    gpioSetPullup ( &IOCON_PIO1_4, gpioPullupMode_PullUp );

    IOCON_PIO3_2 &= ~IOCON_PIO3_2_FUNC_MASK;
    IOCON_PIO3_2 |= IOCON_PIO3_2_FUNC_GPIO;
    gpioSetDir ( CFG_LCD_DATA_PORT, CFG_LCD_SID, gpioDirection_Output );
    gpioSetPullup ( &IOCON_PIO3_2, gpioPullupMode_PullUp );

    IOCON_PIO1_11 &= ~IOCON_PIO1_11_FUNC_MASK;
    IOCON_PIO1_11 |= IOCON_PIO1_11_FUNC_GPIO;
    gpioSetDir ( CFG_LCD_SCLK_PORT, CFG_LCD_SCLK, gpioDirection_Output );
    gpioSetPullup ( &IOCON_PIO1_11, gpioPullupMode_PullUp );

    clrbit ( CFG_LCD_CTRL_PORT, CFG_LCD_RST ); //低电平有效

    delayms ( 100 );
    setbit ( CFG_LCD_CTRL_PORT, CFG_LCD_RST ); //低电平有效
    // TODO: set PSB to 0, enable serial
    clrbit ( CFG_LCD_SCLK_PORT, CFG_LCD_SCLK ); // set to clear ? need check
    clrbit ( CFG_LCD_DATA_PORT, CFG_LCD_SID ); // set to clear ? need check

    clrbit ( CFG_LCD_CTRL_PORT, CFG_LCD_RS );

}


//************************************************************************************
//*函数名称:void LCD_Write(unsigned char Data_Command , unsigned char uc_Content ) *
//*函数功能:并行模式向LCD发送数据或指令												 *
//*形式参数:unsigned char Data_Command , unsigned char uc_Content					 *
//*形参说明:数据或指令的标志位,指令或数据的内容										 *
//*返回参数:无																		 *
//*使用说明:无																		 *
//************************************************************************************
void LCD_Write ( unsigned char Data_Command , unsigned char uc_Content )
{

    unsigned char temp;
    clrbit ( CFG_LCD_SCLK_PORT, CFG_LCD_SCLK );

    setbit ( CFG_LCD_CTRL_PORT, CFG_LCD_RS );

    if ( Data_Command )
    {
        temp = 0xFA;	//RS = 1,数据; RW = 0,写模式
    }
    else
    {
        temp = 0xF8;	//RS = 0,指令; RW = 0,写模式
    }
    out_data ( temp );

    temp = uc_Content & 0xF0;
    out_data ( temp );

    temp = ( uc_Content << 4 ) & 0xF0;
    out_data ( temp );

    clrbit ( CFG_LCD_SCLK_PORT, CFG_LCD_SCLK );

    clrbit ( CFG_LCD_CTRL_PORT, CFG_LCD_RS );
}

//********************************************************
//*函数名称:void LCD_ClearRam(void)	             *
//*函数功能:并行清屏函数					             *
//*形式参数:无								             *
//*形参说明:无							     	         *
//*返回参数:无								             *
//*使用说明:DDRAM填满20H,并设定DDRAM AC到00H	         *
//*格式说明:											 *
//*		    RS	RW	DB7	 DB6  DB5  DB4	DB3	DB2	DB1	DB0	 *
//*		    0	0	 0	  0	   0	0	 0   0	 0	 1	 *
//********************************************************
void LCD_ClearRam ( void )
{
    LCD_Write ( LCD_COMMAND, LCD_BASIC_FUNCTION );			//基本指令集
    LCD_Write ( LCD_COMMAND, LCD_CLEAR_SCREEN );			//清屏
    delayms ( 10 );
}


//*************************************************************
//*函数名称:void LCD_DDRAM_AddressSet(unsigned char ucDDramAdd) *
//*函数功能:设定DDRAM(文本区)地址到地址计数器AC				  *
//*形式参数:unsigned char ucDDramAdd								  *
//*行参说明:具体地址										  *
//*格式说明:												  *
//*		RS	RW	DB7	DB6	DB5	DB4	DB3	DB2	DB1	DB0				  *
//*		0	0	1	AC6	AC5	AC4	AC3	AC2	AC1	AC0				  *
//*返回参数:无												  *
//*使用说明:												  *
//*	第一行地址:80H~8FH										  *
//*	第二行地址:90H~9FH										  *
//*	第三行地址:A0H~AFH										  *
//*	第四行地址:B0H~BFH										  *
//*************************************************************
void LCD_DDRAM_AddressSet ( unsigned char ucDDramAdd )
{
    LCD_Write ( LCD_COMMAND, LCD_BASIC_FUNCTION );			//基本指令集
    LCD_Write ( LCD_COMMAND, ucDDramAdd );				//设定DDRAM地址到地址计数器AC
}

uint32_t lpc_strlen ( const char *pString )
{
    unsigned int length = 0;

    while ( *pString++ != 0 )
    {
        length++;
    }
    return length;
}
//*********************************************************************************
//*函数名称:void LCD_DisplayChinese(unsigned char x,unsigned char y,unsigned char code *p)	  *
//*函数功能:在(文本区)ucAdd指定的位置显示一串字符(或是汉字或是ASCII或是两者混合)  *
//*形式参数:unsigned char x,unsigned char y,const char *p						  *
//*形参说明:指定的位置,要显示的字符串											  *
//*地址必须是: x<8, y<4			                       						  *
//*返回参数:无																	  *
//*使用说明:使用之前要初始化液晶												  *
//*********************************************************************************
void LCD_DisplayChinese ( unsigned char x, unsigned char y, const char *p )
{
    unsigned char length, addr;

    y &= 0x03;	//y < 4
    x &= 0x07;	//x < 8

    switch ( y )
    {
    case 0:
        addr = 0x80;
        break;

    case 1:
        addr = 0x90;
        break;

    case 2:
        addr = 0x88;
        break;

    case 3:
        addr = 0x98;
        break;
    }

    addr += x;

    length = lpc_strlen ( p );

    LCD_Write ( LCD_COMMAND, LCD_BASIC_FUNCTION );  			//基本指令动作
    LCD_DDRAM_AddressSet ( addr );

    for ( ;  length ; length -- )
        LCD_Write ( LCD_DATA, * ( p++ ) );
}
//*********************************************************************************
//*函数名称:void LCD_DisplayStrings_WithAddress(unsigned char ucAdd,unsigned char code *p)	  *
//*函数功能:在(文本区)ucAdd指定的位置显示一串字符(或是汉字或是ASCII或是两者混合)  *
//*形式参数:unsigned char ucAdd,unsigned char code *p							  *
//*形参说明:指定的位置,要显示的字符串											  *
//*地址必须是:80H~87H,90H~97H,88H~8FH,98H~8FH									  *
//*返回参数:无																	  *
//*使用说明:使用之前要初始化液晶												  *
//*********************************************************************************
void LCD_DisplayStrings_WithAddress ( unsigned char ucAdd, const char *p )
{
    unsigned char length;
    length = lpc_strlen ( p );
    LCD_Write ( LCD_COMMAND, LCD_BASIC_FUNCTION );  			//基本指令动作
    LCD_DDRAM_AddressSet ( ucAdd );
    for ( ;  length ; length -- )
        LCD_Write ( LCD_DATA, * ( p++ ) );
}



//*********************************************************************************
//*函数名称:void LCD_DisplayStrings(unsigned char x,unsigned char y,unsigned char code *p)	  *
//*函数功能:在(文本区)ucAdd指定的位置显示一串ASCII                                *
//*形式参数:unsigned char x,unsigned char y,const char *p						  *
//*形参说明:指定的位置,要显示的字符串											  *
//*地址必须是: x<16, y<4			                       						  *
//*返回参数:无																	  *
//*使用说明:使用之前要初始化液晶												  *
//*********************************************************************************
void LCD_DisplayStrings ( unsigned char x, unsigned char y, const char *p )
{
    unsigned char length, addr;
    unsigned char AddrEO;	//x地址是奇偶
    unsigned char Data1;

    y &= 0x03;	//y < 4
    x &= 0x0F;	//x < 16

    switch ( y )
    {
    case 0:
        addr = 0x80;
        break;

    case 1:
        addr = 0x90;
        break;

    case 2:
        addr = 0x88;
        break;

    case 3:
        addr = 0x98;
        break;
    }

    addr += x / 2;
    AddrEO = x & 0x01;

    length = lpc_strlen ( p );

    LCD_Write ( LCD_COMMAND, LCD_BASIC_FUNCTION );  			//基本指令动作
    LCD_DDRAM_AddressSet ( addr );

    /*串行方式下，只支持偶数字节读取，不支持奇地址，
    如果出现奇地址情况，则清楚其偶地址中的数据*/
    if ( AddrEO == 0x01 )
    {
        Data1 = 0x20;
        LCD_DDRAM_AddressSet ( addr );
        LCD_Write ( LCD_DATA, Data1 );
    }

    for ( ;  length ; length -- )
        LCD_Write ( LCD_DATA, * ( p++ ) );
}
//显示32位16进制数字
void LCD_DisplayUINT32 ( unsigned char x, unsigned char y, unsigned long val )
{
	char data[10] = {0};
	int j=0;
	int i=0;
	unsigned char ch=0;
	for(i=0; i<32;i+=4){
		ch = (val>>(28-i))&0x0F;
		if (ch<10)
                data[j++]=ch+'0';  //转换成ASCII码
        else  
                data[j++]=ch-0x0a+'A';
        
	}
	LCD_DisplayStrings(x, y, data);
}
//显示255以内十进制数字
void LCD_DisplayDEC10 ( unsigned char x, unsigned char y, unsigned char val )
{
	char data[10] = {0};
	char hchar, mchar, lchar;

	hchar = val / 100 ;
	val = val - hchar*100;
	mchar = val/10;
	lchar = val - mchar*10;
	data[2] =  lchar +48; //转换成ASCII码，48=‘0’
	data[1] =mchar +48;	 
	data[0] =hchar +48;	

	LCD_DisplayStrings(x, y, data);
}

//**********************************************************
//函数名称:void Serial_UpRollOneLine(unsigned char ucN)
//函数功能:DDRAM(文本区)内容垂直上卷ucN行
//形式参数:unsigned char ucN
//行参说明:上卷的行数:0~3
//返回参数:无
//使用说明:无
//注意：该函数不是非常好用，请尽量不要使用
//**********************************************************
void LCD_UpRollOneLine ( unsigned char ucN )
{
    LCD_Write ( LCD_COMMAND, LCD_EXTEND_FUNCTION );  	//扩展指令集
    LCD_Write ( LCD_COMMAND, LCD_ROLLADDRESS_ON );  		//允许输入卷动位址
    LCD_Write ( LCD_COMMAND, 0x40 + ucN * 0x10 );  		//上卷ucN行
}

/*GDRam */


//*******************************************************************************
//*函数名称:void LCD_GDRAM_AddressSet(unsigned char ucGDramAdd)                   *
//*函数功能:设定GDRAM(图形区)地址到地址计数器AC                                 *
//*形式参数:unsigned char ucGDramAdd                                                    *
//*形参说明:具体地址值                                                          *
//*		RS	RW	DB7	DB6	DB5	DB4	DB3	DB2	DB1	DB0                                 *
//*		0	0	1	AC6	AC5	AC4	AC3	AC2	AC1	AC0                                 *
//*		先设定垂直位置再设定水平位置(连续写入两个字节完成垂直和水平位置的设置)  *
//*		垂直地址范围:AC6~AC0                                                    *
//*		水平地址范围:AC3~AC0                                                    *
//*返回参数:无                                                                  *
//*使用说明:必须在扩展指令集的情况下使用                                        *
//*******************************************************************************
void LCD_GDRAM_AddressSet ( unsigned char ucGDramAdd )
{
    LCD_Write ( LCD_COMMAND, LCD_EXTEND_FUNCTION );			//扩展指令集
    LCD_Write ( LCD_COMMAND, ucGDramAdd );
}



//*****************************************************
//*函数名称:void LCD_GraphModeSet(unsigned char bSelect) *
//*函数功能:打开或关闭绘图显示						  *
//*形式参数:unsigned char Select					  *
//*形参说明:打开或关闭绘图显示的标志位,1打开,0关闭	  *
//*格式说明:										  *
//*		RS	RW	DB7	DB6	DB5	DB4	DB3	DB2	DB1	DB0		  *
//*		0	0	0	0	1	1	X	RE	 G	 X		  *
//*		                    						  *
//*		RE:0,基本指令集								  *
//*		RE:1,扩充指令集								  *
//*		G :0,绘图显示OFF							  *
//*		G :1,绘图显示ON								  *
//*返回参数:无										  *
//*使用说明:无										  *
//*****************************************************
void LCD_GraphModeSet ( unsigned char Select )
{
    LCD_Write ( LCD_COMMAND, LCD_EXTEND_FUNCTION );	//扩展指令集
    if ( Select )
    {
        LCD_Write ( LCD_COMMAND, LCD_GRAPH_ON );		//打开绘图模式
    }
    else
    {
        LCD_Write ( LCD_COMMAND, LCD_GRAPH_OFF );		//关闭绘图模式
    }
}

void LCD_DisplayCLR()
{
    unsigned char i, j;
    LCD_GraphModeSet ( 0x00 ); //先关闭图形显示功能
    for ( j = 0; j < 32; j++ )
    {
        for ( i = 0; i < 8; i++ )
        {
            LCD_Write ( LCD_COMMAND, 0x80 + j );		//设定垂直坐标
            LCD_Write ( LCD_COMMAND, 0x80 + i );		//设定水平坐标
            LCD_Write ( LCD_DATA, 0x00 );	//放入数据高字节
            LCD_Write ( LCD_DATA, 0x00 );	//放入数据低字节
        }
    }
    for ( j = 32; j < 64; j++ )
    {
        for ( i = 0; i < 8; i++ )
        {
            LCD_Write ( LCD_COMMAND, 0x80 + j - 32 );
            LCD_Write ( LCD_COMMAND, 0x88 + i );
            LCD_Write ( LCD_DATA, 0x00 );
            LCD_Write ( LCD_DATA, 0x00 );
        }
    }
    LCD_GraphModeSet ( 0x01 ); //最后打开图形显示功能
}

//*********************************************************************
//*函数名称:void LCD_ImgDisplay(unsigned char code *img)         *
//*函数功能:全屏显示128*64个象素的图形                                *
//*形式参数:unsigned char code *img                                   *
//*形参说明:要显示的图形                                              *
//*液晶屏坐标说明:                                                    *
//*      ________________128个像素______________________              *
//*      |(0,0)                                    (7,0)|             *
//*      |                                              |             *
//*   64 |                                              |             *
//*   个 |(0,31)                                  (7,31)|             *
//*   像 |(8,0)                                   (15,0)|             *
//*   素 |                                              |             *
//*      |                                              |             *
//*      |(8,31)                                 (15,31)|             *
//*      |______________________________________________|             *
//*                                                                   *
//返回参数:无                                                         *
//使用说明:此函数适用于CM12864-12型液晶                               *
//*********************************************************************
void LCD_ImgDisplay ( unsigned char *img )
{
    unsigned char i, j;
    LCD_GraphModeSet ( 0x00 ); //先关闭图形显示功能
    for ( j = 0; j < 32; j++ )
    {
        for ( i = 0; i < 8; i++ )
        {
            LCD_Write ( LCD_COMMAND, 0x80 + j );		//设定垂直坐标
            LCD_Write ( LCD_COMMAND, 0x80 + i );		//设定水平坐标
            LCD_Write ( LCD_DATA, img[j * 16 + i * 2] );	//放入数据高字节
            LCD_Write ( LCD_DATA, img[j * 16 + i * 2 + 1] );	//放入数据低字节
        }
    }
    for ( j = 32; j < 64; j++ )
    {
        for ( i = 0; i < 8; i++ )
        {
            LCD_Write ( LCD_COMMAND, 0x80 + j - 32 );
            LCD_Write ( LCD_COMMAND, 0x88 + i );
            LCD_Write ( LCD_DATA, img[j * 16 + i * 2] );
            LCD_Write ( LCD_DATA, img[j * 16 + i * 2 + 1] );
        }
    }
    LCD_GraphModeSet ( 0x01 ); //最后打开图形显示功能
}


//******************************************************************************
//*函数名称:void LCD_ImgDisplayCharacter(unsigned char x,unsigned char y,unsigned char code *img) *
//*函数功能:使用绘图的方法,在(x,y)处画一个16*16点阵的图案,也可以是字符		   *
//*形式参数:unsigned char x,unsigned char y,unsigned char code *img									   *
//*			x取值范围:0~7													   *
//*			y取值范围:0~31 (针对CM12864-12型液晶)						       *
//*形参说明:坐标水平位置,坐标垂直位置,要显示的图形							   *
//*液晶屏坐标说明:															   *
//*      ________________128个像素______________________              *
//*      |(0,0)                                    (7,0)|             *
//*      |                                              |             *
//*   64 |                                              |             *
//*   个 |(0,31)                                  (7,31)|             *
//*   像 |(8,0)                                   (15,0)|             *
//*   素 |                                              |             *
//*      |                                              |             *
//*      |(8,31)                                 (15,31)|             *
//*      |______________________________________________|             *
//*                                                                   *
//*返回参数:无                                                              *
//*使用说明:此函数适用于CM12864-12型液晶			*
//******************************************************************************
void LCD_ImgDisplayCharacter ( unsigned char x, unsigned char y, unsigned char *img )
{
    unsigned char i;
    LCD_GraphModeSet ( 0x00 );		//先关闭图形显示功能
    LCD_Write ( LCD_COMMAND, LCD_EXTEND_FUNCTION );
    for ( i = 0; i < 16; i++ )
    {
        LCD_GDRAM_AddressSet ( 0x80 + y + i );
        LCD_GDRAM_AddressSet ( 0x80 + x );
        LCD_Write ( LCD_DATA, img[i * 2] );
        LCD_Write ( LCD_DATA, img[i * 2 + 1] );
    }
    LCD_GraphModeSet ( 0x01 );				//最后打开图形显示功能
}


/**********************液晶文字显示相关程序END*************/
