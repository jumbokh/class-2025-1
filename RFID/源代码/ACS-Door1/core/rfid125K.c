#include "LPC11xx.h"     
#include "gpio.h"
#include "lcd.h"

extern void beepOn (void);
extern void beepOff (void);
extern void delayms (uint32_t ulTime);



/* 125k IO definitions */
#define VCCN_125K_PORT 	2
#define VCCN_125K_PIN  	6
#define VCCN_125K_IOCON  	IOCON_PIO2_6  //125KHz阅读器模块电源控制端口

#define WG1_125K_PORT 2
#define WG1_125K_PIN  4
#define WG1_125K_IOCON IOCON_PIO2_4		 //WG"0"数据端口

#define WG2_125K_PORT 1
#define WG2_125K_PIN  8
#define WG2_125K_IOCON IOCON_PIO1_8	   //WG"1"数据端口

#define ACT_FLAG_125K_PORT  3
#define ACT_FLAG_125K_PIN   4
#define ACT_FLAG_125K_IOCON IOCON_PIO3_4   //WG数据接收标志位

			

#define CARD_125K_MAX_BITS   34		 //韦根数据位数
uint8_t cardId_125k[CARD_125K_MAX_BITS / 8 + 1] = {0}; //wg-34bit, only 4 byte card id（数组定义，34位数据用5个字节表示，初始值为0）
uint32_t CardID = 0;  //32位卡ID，初始值为0
int cardId_bits = 0;  //接收34bit韦根数据的计数变量
uint32_t cardSts1 =1;  //刷卡状态变量


/* wg1_125K, 0 is received
   wg2_125K, 1 is received，以中断方式接收WG信号*/
void wg_isr_handler ( int bitVal )	 //中断服务函数
{
    uint32_t cardSts = gpioGetValue ( ACT_FLAG_125K_PORT, ACT_FLAG_125K_PIN ); //有无卡判断，0: has card; 1:no card

    if ( !cardSts )	  //有卡
    {
        if ( cardId_bits > CARD_125K_MAX_BITS )	 //接收完34BIT数据便不再接收数据
            return;
        cardId_125k[cardId_bits / 8] |=  ( bitVal ) << ( 7 - ( cardId_bits % 8 ) );	//收到的数据都移到cardId_125k数组中， cardId_125k数组是全局变量
        cardId_bits++;	//中断1次卡ID位数加1
    }
    return;
}


//======================
//configure the GPIO pins for RFID125K 
//======================
void init_125k_module()
{
	// 打开模块电源
    VCCN_125K_IOCON &= ~IOCON_PIO2_6_FUNC_MASK;
    VCCN_125K_IOCON |= IOCON_PIO2_6_FUNC_GPIO;
    gpioSetDir ( VCCN_125K_PORT, VCCN_125K_PIN, gpioDirection_Output );
    gpioSetValue ( VCCN_125K_PORT, VCCN_125K_PIN, 1 );	//阅读器模块电源控制端口 “1” ，关闭电源

	 //	  WG1引脚上拉，WG引脚设置
    WG1_125K_IOCON &= ~IOCON_PIO2_4_FUNC_MASK;
    WG1_125K_IOCON |= IOCON_PIO2_4_FUNC_GPIO;
    gpioSetDir ( WG1_125K_PORT, WG1_125K_PIN, gpioDirection_Input );
    gpioSetPullup ( &WG1_125K_IOCON, gpioPullupMode_Inactive );

    // configure pin for interrupt ，允许WG1中断
    gpioSetInterrupt ( WG1_125K_PORT,
                       WG1_125K_PIN,
                       gpioInterruptSense_Edge,        // level-sensitive
                       gpioInterruptEdge_Single,       // invalid
                       gpioInterruptEvent_ActiveHigh ); // High triggers interrupt


	//	  WG2引脚上拉，WG引脚设置
    WG2_125K_IOCON &= ~IOCON_PIO1_8_FUNC_MASK;
    WG2_125K_IOCON |= IOCON_PIO1_8_FUNC_GPIO;
    gpioSetDir ( WG2_125K_PORT, WG2_125K_PIN, gpioDirection_Input );
    gpioSetPullup ( &WG2_125K_IOCON, gpioPullupMode_Inactive );

    // configure pin for interrupt ，允许WG2中断
    gpioSetInterrupt ( WG2_125K_PORT,
                       WG2_125K_PIN,
                       gpioInterruptSense_Edge,        // level-sensitive
                       gpioInterruptEdge_Single,		// invalid	非法的
                       gpioInterruptEvent_ActiveHigh ); // High triggers interrupt

    // enable interrupt
    gpioIntClear ( WG1_125K_PORT, WG1_125K_PIN );
    gpioIntEnable ( WG1_125K_PORT, WG1_125K_PIN );

    gpioIntClear ( WG2_125K_PORT, WG2_125K_PIN );  
    gpioIntEnable ( WG2_125K_PORT, WG2_125K_PIN );

	 //WG状态引脚上拉设置
    ACT_FLAG_125K_IOCON &= ~IOCON_PIO3_4_FUNC_MASK;
    ACT_FLAG_125K_IOCON |= IOCON_PIO3_4_FUNC_GPIO;
    gpioSetDir ( ACT_FLAG_125K_PORT, ACT_FLAG_125K_PIN, gpioDirection_Input );
    gpioSetPullup ( &ACT_FLAG_125K_IOCON, gpioPullupMode_Inactive );

}

//====================
//Enable GPIOs for RFID 125K module
//===================
void enable_125k_module()
{

    gpioSetValue ( VCCN_125K_PORT, VCCN_125K_PIN, 0 ); //open the power

    gpioSetPullup ( &WG1_125K_IOCON, gpioPullupMode_PullUp );

    gpioSetPullup ( &WG2_125K_IOCON, gpioPullupMode_PullUp );

    gpioSetPullup ( &ACT_FLAG_125K_IOCON, gpioPullupMode_PullUp );

}
 
//====================
//Disable GPIOs for RFID 125K module
//===================
void disable_125k_module()
{
    gpioSetPullup ( &WG1_125K_IOCON, gpioPullupMode_Inactive );

    gpioSetPullup ( &WG2_125K_IOCON, gpioPullupMode_Inactive );

    gpioSetPullup ( &ACT_FLAG_125K_IOCON, gpioPullupMode_Inactive );

    gpioSetDir ( VCCN_125K_PORT, VCCN_125K_PIN, gpioDirection_Output );
    gpioSetValue ( VCCN_125K_PORT, VCCN_125K_PIN, 1 );//关闭电源

}

//=======================
// Valid(验证)the Card ID
//=======================
uint32_t checkCardID ( uint8_t *data )
{
    uint8_t odd = 0;
    uint8_t even = 0;
    int i = 0;
    int parity = 0;

    odd = data[0] ^ data[1] ^ ( data[2] >> 7 );

    //check the odd parity
    i = 8;
    parity = 0;
    while ( i > 0 && odd )
    {
        if ( odd & 0x01 )
        {
            parity++;
        }
        odd >>= 1;
        i--;
    }
    if ( ( parity & 0x01 ) == 0 )
    {
        return 0;
    }

    //check the even parity
    even = data[3] ^ data[4] ^ ( data[2] << 1 );
    i = 8;
    parity = 0;
    while ( i > 0 && even )
    {
        if ( even & 0x01 )
        {
            parity++;
        }
        even >>= 1;
        i--;
    }
    if ( ( parity & 0x01 ) )
    {
        return 0;
    }

//将接收到的34bit韦根数据中32bit卡的ID取出，并赋值给CardID
    CardID = 0;

    CardID |= ( data[0] << 1 ) | ( data[1] >> 7 );//34bit数据中第1个字节左移一位，将偶检验位移出，并与第2个字节的第1位或，得到卡ID的第1个字节

    CardID = ( CardID << 8 ) | ( ( data[1] << 1 ) | ( data[2] >> 7 ) );//得到卡ID的第2个字节

    CardID = ( CardID << 8 ) | ( ( data[2] << 1 ) | ( data[3] >> 7 ) );//得到卡ID的第3个字节

    CardID = ( CardID << 8 ) | ( ( data[3] << 1 ) | ( data[4] >> 7 ) );//得到卡ID的第4个字节

    return CardID;
}




//=======================
//刷卡响铃指示
//显示已刷卡信息
//
//=======================
uint32_t DisplayCardID()
{	    

		//有卡
	    beepOn();
		Led2On();
    	delayms ( 100 );
    	beepOff();	//蜂鸣器响一声
		Led2Off();

        LCD_DisplayChinese ( 0, 2,  "已刷卡，ID号:  " )	;
		LCD_DisplayUINT32(0, 3, CardID);  //显示卡的ID号
		
        
    }
 

// 将数值value写入缓冲区pBuffer							 
void * lpc_memset ( void *pBuffer, int value, uint32_t num )
{
    unsigned char *pByteDestination;
    unsigned int  *pAlignedDestination = ( unsigned int * ) pBuffer;
    unsigned int  alignedValue = ( value << 24 ) | ( value << 16 ) | ( value << 8 ) | value;

    // Set words if possible
    if ( ( ( ( unsigned int ) pAlignedDestination & 0x3 ) == 0 ) && ( num >= 4 ) )
    {
        while ( num >= 4 )
        {
            *pAlignedDestination++ = alignedValue;
            num -= 4;
        }
    }
    // Set remaining bytes
    pByteDestination = ( unsigned char * ) pAlignedDestination;
    while ( num-- )
    {
        *pByteDestination++ = value;
    }
    return pBuffer;
}


//==============================
//循环查询刷卡状况并显示刷卡信息(BEEP响铃提示)
//
//==============================
void rfid125k_read ()
{

	/*配置125K管脚，及中断*/
	init_125k_module();

	/*开电125k模块*/
	enable_125k_module();

	while(1)
	{

		if ( cardId_bits >= CARD_125K_MAX_BITS )   //判断是否接收完34bit数据
	    {
	        
	        checkCardID ( cardId_125k );  //验证和获得卡ID
		
	        DisplayCardID(); 	//刷卡后动作，响铃、LED、ID显示等				   	  
	    	
			delayms ( 1000 );
	
			//进入等待下一次的状态
	        LCD_ClearRam();	
			LCD_DisplayChinese ( 0, 0,  "欢迎使用门禁系统" );
			LCD_DisplayChinese ( 0, 1,  "请刷卡  " )	;
	 		
			lpc_memset ( cardId_125k, 0, sizeof ( cardId_125k ) );	 //将保存卡ID的缓冲区清0，准备重新刷卡接收卡ID
	     
	        cardId_bits = 0;   //清除上一次刷卡记录	，准备重新接收韦根数据
	
	     }
   	}
 }


//判断有无刷卡并响铃提示
void rfid125k_read2 ()
{

	/*配置125K管脚，及中断*/
	init_125k_module();

	/*开电125k模块*/
	enable_125k_module();

   	while(1)
	{
	  	cardSts1 = gpioGetValue ( ACT_FLAG_125K_PORT, ACT_FLAG_125K_PIN ); //有无卡判断，0: has card; 1:no card
		
		if ( !cardSts1 )
	    {
			//有卡
			LCD_DisplayChinese ( 0, 1,  "已刷卡  " )	;
		    beepOn();
			Led2On();
	    	delayms ( 100 );
	    	beepOff();	//蜂鸣器响一声
			Led2Off();

		   	delayms ( 500 );

			//进入等待下一次的状态
			LCD_ClearRam();	//清屏
			LCD_DisplayChinese ( 0, 0,  "欢迎使用门禁系统" );
			LCD_DisplayChinese ( 0, 1,  "请刷卡  " )	;
		
		  }
	   }
	}
