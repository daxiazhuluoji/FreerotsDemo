/**
 * Copyright (C) 2018
 * All Rights Reserved
 * 
 * Filename     : rf.c
 * Author       : daxiazhuluoji
 * Last modified: 2018-09-03 10:36
 * Email        : lidianjun@yeah.net
 * Description  : 
 * 
 */


#if 0
#define HS6206_DEVICE
//#define _HS6206_USE_FPGA_ 			//使用FPGA+HS6200做仿真调试时, 必须打开这个编译开关. 实际OTP运行时,关掉.
//#define HS6206_POWER_DOWN_EANBLE	//当程序使用了睡眠模式,必须打开这个编译开关
#elif 0
#define HS6207_DEVICE
//#define _HS6207_REG_CTRL_CE_		//使用内部寄存器控制CE时,必须打开这个编译开关, 同时CE脚必须置高电平.
#else
#define HS6200_DEVICE
#endif

#include "RF.h"


#ifdef HS6206_DEVICE
#include "HS6206_Regs.h"
#include "hs6206_Uti.h"
#include "HS6206_Delay.h"
#include "stdio.h"

#define RF_CE_HIGH       RFCON |= RFSPI_CE
#define RF_CE_LOW        RFCON &= ~RFSPI_CE
#define RF_DELAY_20us    delay_us(20)

#define RF_CE_HIGH_LOW   do{RF_CE_HIGH;RF_DELAY_20us;RF_CE_LOW;}while(0)

#define RF_CSN_HIGH      RFCON |= RFSPI_CSN  
#define RF_CSN_LOW       RFCON &= ~RFSPI_CSN

#define CODE code
#define XDATA xdata

#endif

#ifdef HS6207_DEVICE

#include "stdio.h"
#include "stdint.h"
#include "config_HS6207.h"
#include "HS6207_GPIO.h"
#include "HS6207_SPI.h"
#include "delay.h"

#define RF_CE_HIGH	    (GPIO_GRUP_0->DOSET = GPIO_PIN_7)
#define RF_CE_LOW       (GPIO_GRUP_0->DOCLR = GPIO_PIN_7)
#define RF_DELAY_1ms     my_delay_ms(1)
#define RF_CE_HIGH_LOW   do{RF_CE_HIGH;RF_DELAY_1ms;RF_CE_LOW;}while(0)
#define RF_CSN_HIGH	    (GPIO_GRUP_2->DOSET = GPIO_PIN_2)
#define RF_CSN_LOW      (GPIO_GRUP_2->DOCLR = GPIO_PIN_2)
#define	RF_IRQ_HIGH		  (GPIO_GRUP_0->DI & GPIO_PIN_5)

#define Delay1ms(n)      my_delay_ms(n)
#define Delay10ms(n)     my_delay_ms(10*n)
#define CODE
#define XDATA
#endif

#ifdef HS6200_DEVICE
#include "delay.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "debug.h"

#define RF_DELAY_1ms     my_delay_ms(1)
#define Delay1ms(n)      my_delay_ms(n)
#define Delay10ms(n)     my_delay_ms(10*n)

#define RF_CE_HIGH	   	GPIO_SetBits(GPIOC,GPIO_Pin_4)
#define RF_CE_LOW       GPIO_ResetBits(GPIOC,GPIO_Pin_4)
#define RF_CE_HIGH_LOW  do{RF_CE_HIGH;RF_DELAY_1ms;RF_CE_LOW;}while(0)
#define RF_CSN_HIGH	    GPIO_SetBits(GPIOA,GPIO_Pin_4)
#define RF_CSN_LOW      GPIO_ResetBits(GPIOA,GPIO_Pin_4)

#define CODE
#define XDATA
#endif

/*Calibration config infor*/
CODE const unsigned char RF_Calibration_Data[] = 
{
	/* Register Addr							DataLenth   Data*/
	RF_BANK0_CONFIG,		1,		0x03,
	RF_BANK0_RF_CH,			1,		0x32,
	RF_BANK0_RF_SETUP,	1,		0x40,
	0xFF	/*Addr=0xFF,complete flag*/ 
};

CODE const unsigned char RF_Calibration_Analog[] = 
{
	/*Register Addr								DataLenth	Data*/ 
	RF_BANK1_PLL_CTL0,  4,		0x40,0x01,0x10,0xE5,
	RF_BANK1_CAL_CTL,		5,		0x20,0x08,0x50,0x40,0x50,
	RF_BANK1_IF_FREQ,		3,		0x00,0x00,0x1F,			
	RF_BANK1_FDEV,			1,		0x25,
	RF_BANK1_DAC_CAL_HI,1,		0x7f,	
	RF_BANK1_RF_IVGEN,	4,		0x1f,0x64,0x00,0x81,
	0xFF	/*Addr=0xFF,complete flag*/
};

CODE const unsigned char RF_Init_Analog[]=
{
	RF_BANK1_PLL_CTL0,		4,	0x40,0x01,0x30,0xE1,  
	RF_BANK1_PLL_CTL1,		4,	0x00,0x42,0x10,0x07,
	RF_BANK1_CAL_CTL,			5,	0x29,0x89,0x75,0x28,0x50,
	RF_BANK1_RX_CTRL,			4,	0x52,0xC2,0x09,0xAC,
	RF_BANK1_FAGC_CTRL_1,	4,	0x80,0x14,0x08,0x29,
	RF_BANK1_RF_IVGEN,		4,	0x1F,0x64,0x00,0x11,
	0xFF	/*Addr=0xFF,complete flag*/
};

/*config infor*/
CODE const unsigned char RF_Init_Data[]=
{
	/*Register Addr								DataLenth	Data*/  

	RF_BANK0_CONFIG,				1,		0x0e,
	RF_BANK0_RX_PW_P0,			1,		0x0A,
	RF_BANK0_DYNPD,					1,		0x3f,		
	RF_BANK0_FEATURE,				1,		0x07,	
	RF_BANK0_SETUP_VALUE, 	5,		0x40,0x5A,0x80,0x06,0x00,
	RF_BANK0_PRE_GURD,		  1,	  0x77,
	RF_BANK0_EN_AA,					1,		0x3f,
	RF_BANK0_EN_RXADDR,			1,		0x01,
	RF_BANK0_SETUP_AW,			1,		0x03,
	RF_BANK0_SETUP_RETR,		1,		0x0f,
	RF_BANK0_RF_CH,					1,		0x1A,
	RF_BANK0_RF_SETUP,			1,		0x47,
	RF_BANK0_RX_ADDR_P0,		5,		0x62,0x54,0x79,0x38,0x53,
	RF_BANK0_TX_ADDR,				5,		0x62,0x54,0x79,0x38,0x53,
	0xFF	/*Addr=0xFF,complete flag*/

};


CODE const unsigned char RF_Tx_Power[8][5] = 
{
	//Bank0           Bank1
	//RF_SETUP     RF_IVGEN
	0x00,0x9F,0x64,0x00,0x01,//-43dBm
	0x00,0x1F,0x64,0x00,0x01,//-16dBm
	0x01,0x9F,0x64,0x00,0x01,//-12dBm
	0x01,0x1F,0x64,0x00,0x01,//-6dBm

	0x03,0x1F,0x64,0x00,0x01,//0dBm
	0x07,0x9F,0x64,0x00,0x01,//4dBm
	0x40,0x1F,0x64,0x00,0x01,//5dBm
	0x47,0x1F,0x64,0x00,0x01 //8dBm
};

/* Switch register Bank*/
void RF_Bank_Switch(RF_Bank_TypeDef bank)
{
	unsigned char sta;

	sta = RF_read_byte(RF_BANK0_STATUS);

	if(bank != RF_Bank0)
	{
		if(!(sta & RF_BANK1))
		{
			RF_wr_cmd(RF_ACTIVATE, RF_ACTIVATE_DATA);
		}
	}
	else
	{
		if(sta & RF_BANK1)
		{
			RF_wr_cmd(RF_ACTIVATE, RF_ACTIVATE_DATA);
		}
	}
}

void RF_CE_High(void)
{
#ifdef _HS6207_REG_CTRL_CE_
	unsigned char preGurd_status[2];
	preGurd_status[0] = 0x77;
	preGurd_status[1] = 0x00;
	RF_wr_buffer(RF_BANK0_PRE_GURD, preGurd_status, 2);
#else
	RF_CE_HIGH;
#endif	
}

void RF_CE_High_Pulse(void)
{
#ifdef _HS6207_REG_CTRL_CE_
	unsigned char preGurd_status[2];
	preGurd_status[0] = 0x77;
	preGurd_status[1] = 0x00;
	RF_wr_buffer(RF_BANK0_PRE_GURD, preGurd_status, 2); 

	Delay1ms(1);

	preGurd_status[1] = 0x80;
	RF_wr_buffer(RF_BANK0_PRE_GURD, preGurd_status, 2);
#else
	RF_CE_HIGH_LOW;
#endif

}

void RF_CE_Low(void)
{
#ifdef _HS6207_REG_CTRL_CE_
	unsigned char preGurd_status[2];
	preGurd_status[0] = 0x77;
	preGurd_status[1] = 0x80;
	RF_wr_buffer(RF_BANK0_PRE_GURD, preGurd_status, 2);  
#else 
	RF_CE_LOW;
#endif

}

/*Caution: Before calling the 'HS6206_ChangeAddr_Reg', */
/*              you should pull down the CE pin, using 'HS6206_CE_Low'*/
void RF_ChangeAddr_Reg(unsigned char* AddrBuf,unsigned char len)
{
	RF_wr_buffer(RF_BANK0_RX_ADDR_P0, AddrBuf, len);
	RF_wr_buffer(RF_BANK0_TX_ADDR, AddrBuf,len);
}

/*change the channel*/
/*Caution: Before calling the function 'HS6206_Change_CH', */
/*              you must pull down the CE pin, just call the               */
/*              function HS6206_CE_Low()                                 */

void RF_Change_CH(unsigned char ch_index)
{
	RF_write_byte(RF_BANK0_RF_CH, ch_index);	

}

/*RF Tx changes itself transmitting power*/
/*Caution: Before calling the 'HS6206_Change_Pwr', */
/*              you should pull down the CE pin, using HS6206_CE_Low()*/
void RF_Change_Pwr(signed char Pwr_dBm)
{
	unsigned char	Pwr_Sel;

	switch(Pwr_dBm)
	{
		case -43:
			Pwr_Sel = 0;
			break;

		case -16:
			Pwr_Sel = 1;
			break;

		case -12:
			Pwr_Sel = 2;
			break;

		case -6:
			Pwr_Sel = 3;
			break;

		case  0:
			Pwr_Sel = 4;
			break;

		case  4:
			Pwr_Sel = 5;
			break;

		case  5:
			Pwr_Sel = 6;
			break;

		case  8:
			Pwr_Sel = 7;
			break;

		default:
			Pwr_Sel = 4;
			break;
	}

	RF_Bank_Switch(RF_Bank1);
	RF_wr_buffer(RF_BANK1_RF_IVGEN, &RF_Tx_Power[Pwr_Sel][1], 4);
	RF_Bank_Switch(RF_Bank0);
	RF_write_byte(RF_BANK0_RF_SETUP, RF_Tx_Power[Pwr_Sel][0]);

}

/*Clear All Irq*/
void RF_Clear_All_Irq(void)
{
	RF_write_byte(RF_BANK0_STATUS, 0x70);
}

/*Configuration Reg*/
void RF_Configure_Reg(const unsigned char* Dbuf)
{
	unsigned char cnt = 0;
	unsigned char Reg_Addr;
	unsigned char data_lenth;
	const unsigned char *p_data;

	while(1)
	{
		/*Get Reg_Addr*/
		Reg_Addr = Dbuf[cnt];	
		if(0xFF == Reg_Addr)
		{
			break;	/*Reg_Addr Error,configuration is complete,break*/
		}

		/*Get Data_Lenth*/
		cnt += 1;
		data_lenth = Dbuf[cnt];	

		/*Get Data address*/
		cnt += 1;
		p_data = &Dbuf[cnt];	

		/*Write buffer*/ 
		RF_wr_buffer(Reg_Addr, p_data, data_lenth);	
		cnt += data_lenth;
	}
}

/*Flush Rx*/
void RF_Flush_Rx(void)
{
	RF_Operation(RF_FLUSH_RX);
}


/*Flush Tx*/
void RF_Flush_Tx(void)
{
	RF_Operation(RF_FLUSH_TX);
}

unsigned char RF_Get_Chip_ID(void)
{
	unsigned char ReadArr[2];

#ifdef HS6207_DEVICE
	RF_Bank_Switch(RF_Bank1);
	RF_read_buffer(RF_BANK1_LINE, ReadArr, 2);
#endif

#ifdef HS6206_DEVICE
#ifdef _HS6206_USE_FPGA_
	RF_Bank_Switch(RF_Bank1);
	RF_read_buffer(RF_BANK1_LINE, ReadArr, 2);
#else
	ReadArr[0] = CIDH;
	ReadArr[1] = CIDL;
#endif
#endif

	
#ifdef HS6200_DEVICE
	RF_Bank_Switch(RF_Bank1);
	RF_read_buffer(RF_BANK1_LINE, ReadArr, 2);
#endif

	//app_printf("CID = %x %x\r\n",(int)ReadArr[1], (int)ReadArr[0]);
	return ReadArr[1];
}

/*get RSSI*/
/*Caution: To get the value of the RSSI, you should ensure that */
/*              the RF is in Rx mode and the CE pin is high.*/
signed char RF_Get_RSSI(void)
{
	unsigned char ReadArr;

	RF_Bank_Switch(RF_Bank0);
	ReadArr = RF_read_byte(RF_BANK0_RPD);

	return ((signed char)ReadArr);
}

/*Mode_Switch*/
/*Caution: Before calling the function 'HS6206_ModeSwitch', */
/*              you must pull down the CE pin, just call the              */
/*              function HS6206_CE_Low()                                */

void RF_ModeSwitch(RF_ModeTypeDef mod)
{
	unsigned char tmp;

	tmp = RF_read_byte(RF_BANK0_CONFIG);
	if(mod != Rf_PRX_Mode)
	{
		tmp &= 0xFE;
	}
	else
	{
		tmp |= 0x01;
	}
	RF_write_byte(RF_BANK0_CONFIG, tmp);

	if(mod == Rf_Carrier_Mode)
	{
		tmp = 0x80 | RF_read_byte(RF_BANK0_RF_SETUP);
		RF_write_byte(RF_BANK0_RF_SETUP, tmp);
	}

}

/*Operation Commad  function*/
unsigned char RF_Operation(unsigned char opt)
{	
	unsigned char status;

	RF_CSN_LOW;
	status = RF_spi_wrd(opt);
	RF_CSN_HIGH;
	return status;

}

void RF_read_buffer(unsigned char addr, unsigned char* buf, unsigned char len)
{
	RF_CSN_LOW;
	RF_spi_wrd(RF_R_REGISTER|addr);

	while(len--)
	{
		*buf++ = RF_spi_wrd(0);
	}
	RF_CSN_HIGH;

}

/*Read One Register */
unsigned char RF_read_byte(unsigned char addr)
{
	unsigned char rxdata;

	RF_CSN_LOW;
	RF_spi_wrd(RF_R_REGISTER|addr);
	rxdata = RF_spi_wrd(0);
	RF_CSN_HIGH;

	return(rxdata);


}

/*read status*/
unsigned char RF_Read_Status(void)
{	
	unsigned char status;
	status = RF_Operation(RF_READ_STATUS);

	return status;
}

/*buf:data read from rx buffer*/
unsigned char RF_ReceivePack(unsigned char* buf)
{
	unsigned char sta;
	unsigned char fifo_sta;
	unsigned char len;

	sta = RF_Read_Status();

	if(RF_STATUS_RX_DR & sta)
	{
		do
		{
			len = RF_read_byte(RF_R_RX_PL_WID);

			if(len <= RF_FIFO_MAX_PACK_SIZE)
			{
				RF_read_buffer(RF_R_RX_PAYLOAD, buf, len);
			}
			else
			{
				RF_Flush_Rx();
			}

			fifo_sta = RF_read_byte(RF_BANK0_FIFO_STATUS);

			//printf("RX received data = %d, %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\r\n",len,buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7],buf[8],buf[9],buf[10],buf[11],buf[12],buf[13],buf[14],buf[15],buf[16],buf[17],buf[18],buf[19],buf[20],buf[21],buf[22],buf[23],buf[24],buf[25],buf[26],buf[27],buf[28],buf[29],buf[30],buf[31]);
		}while(!(fifo_sta & RF_FIFO_STA_RX_EMPTY));/*not empty continue read out*/ 

		RF_write_byte(RF_BANK0_STATUS, sta);/*clear irq*/
		return len;
	}

	if(sta & (RF_STATUS_RX_DR | RF_STATUS_TX_DS | RF_STATUS_MAX_RT))
	{
		RF_write_byte(RF_BANK0_STATUS, sta);/*clear irq*/
	}

	return 0;
}

/*buf:data buffer; len:[1~32] ; cmd: HS6200_W_TX_PAYLOAD/HS6200_W_TX_PAYLOAD_NOACK*/
void RF_SendPack(unsigned char cmd, unsigned char* buf, unsigned char len)
{
	unsigned char sta;

	sta = RF_Read_Status();
	if(!(sta&RF_STATUS_TX_FULL))
	{
		RF_wr_buffer(cmd, buf, len);
	}
}

void RF_Soft_Rst(void)
{
	unsigned char soft_temp[4];
	RF_Bank_Switch(RF_Bank1);

	RF_read_buffer(RF_BANK1_PLL_CTL1, soft_temp, 4);
	soft_temp[3] |= 0x80;
	RF_wr_buffer(RF_BANK1_PLL_CTL1, soft_temp, 4);

}

/*RFSPI write/read 1 Byte to/from RF */
unsigned char RF_spi_wrd(unsigned char Byte)
{
#ifdef HS6206_DEVICE
	while(!(SPIRSTAT & RFSPI_TX_FIFO_RDY));      //Tx Fifo not ready.
	SPIRDAT = Byte;
	while(!(SPIRSTAT & RFSPI_RX_DATA_RDY));      //Rx Fifo has data.
	return SPIRDAT;
#endif

#ifdef HS6207_DEVICE
	static unsigned char rxd = 0xFE;

	while(SSP0->STAT & 0x00000010);

	SSP0->DAT = Byte;

	while(SSP0->STAT & 0x00000010);

	rxd = SSP0->DAT & 0x00FF;

	return (rxd);
#endif

#ifdef HS6200_DEVICE
	SPI_TypeDef *P;
	//if (SPI1_EN == SPI_PORT)
	P = SPI1;
	//else
	//P = SPI2;
	
	while((P->SR&SPI_I2S_FLAG_TXE)==RESET);  
	P->DR=Byte;    
	while((P->SR&SPI_I2S_FLAG_RXNE)==RESET);
	return  P->DR;
#endif

}

/*set Auto_ACK msg*/
void RF_Write_Ack_Payload(unsigned char PipeNum, unsigned char *pBuf, unsigned char bytes)
{
	unsigned char byte_ctr;

	RF_CSN_LOW;   //拉低
	RF_spi_wrd(RF_W_ACK_PAYLOAD | PipeNum); 
	for(byte_ctr = 0; byte_ctr < bytes; byte_ctr++)	   // then write all byte in buffer(*pBuf)
	{                          
		RF_spi_wrd(*pBuf++);
	}
	RF_CSN_HIGH;			
}

void RF_wr_buffer(unsigned char addr,const unsigned char* buf,unsigned char len)
{
	RF_CSN_LOW;
	RF_spi_wrd(RF_W_REGISTER|addr);
	while(len--)
	{
		RF_spi_wrd(*buf++);
	}
	RF_CSN_HIGH;
}

/*Write One Register*/
void RF_write_byte(unsigned char addr,unsigned char D)
{
	RF_CSN_LOW;
	RF_spi_wrd(RF_W_REGISTER|addr);
	RF_spi_wrd(D);
	RF_CSN_HIGH;
}

/*write Commad  function, cmd = code; D = data*/
void RF_wr_cmd(unsigned char cmd,unsigned char D)
{
	RF_CSN_LOW;
	RF_spi_wrd(cmd);
	RF_spi_wrd(D);
	RF_CSN_HIGH;
}

#if 1
void RF_Init()
{
#ifdef HS6206_POWER_DOWN_EANBLE
	unsigned char  temp[5];
#endif

	RF_Soft_Rst();

#ifdef HS6207_DEVICE
	while(RF_Get_Chip_ID() != 0x01){} /*id indicate the version of chip*/
#endif
		
#ifdef HS6200_DEVICE
	while(RF_Get_Chip_ID() != 0x01){} /*id indicate the version of chip*/
#endif

#ifdef HS6206_DEVICE
#ifdef _HS6206_USE_FPGA_
	while(RF_Get_Chip_ID() != 0x01){} /*id indicate the version of chip*/
#else
	while(RF_Get_Chip_ID() != 0x04){} /*id indicate the version of chip*/
#endif
#endif

	/*Calibration*/
	RF_Bank_Switch(RF_Bank0);
	RF_CE_Low();
	RF_Configure_Reg(RF_Calibration_Data);

	RF_Bank_Switch(RF_Bank1);
	RF_Configure_Reg(RF_Calibration_Analog);

	RF_Bank_Switch(RF_Bank0);
	Delay1ms(2);//waiting for crystal works steadily, delay must > 2ms

	/*CE pluse*/
	RF_CE_High();
	Delay1ms(2);//delay must > 20us
	RF_CE_Low();

	Delay10ms(4);//waitting for calibaration,delay must > 40ms

	RF_Bank_Switch(RF_Bank1);
	RF_Configure_Reg(RF_Init_Analog);

	RF_Bank_Switch(RF_Bank0);
	RF_Configure_Reg(RF_Init_Data);

	RF_Clear_All_Irq();
	RF_Flush_Tx();
	RF_Flush_Rx();

#ifdef HS6206_POWER_DOWN_EANBLE

	/*read and save reg for powerdown without init*/
	RF_Bank_Switch(RF_Bank1);

	RF_read_buffer(RF_BANK1_CAL_CTL, temp, 5);
	temp[0] |=0xc0;
	RF_wr_buffer(RF_BANK1_CAL_CTL, temp, 5);

	RF_read_buffer(RF_BANK1_PLL_CTL0, temp, 5);
	temp[0] |=0x20;
	RF_wr_buffer(RF_BANK1_PLL_CTL0, temp, 5);

	RF_read_buffer(RF_BANK1_DAC_RANGE, temp, 1);

	RF_wr_buffer(RF_BANK1_DAC_RANGE, temp, 1);

	RF_Bank_Switch(RF_Bank0);

#endif
	RF_CE_High();

}

#endif

#if 0
unsigned char RF_Init(void)
{
#ifdef HS6206_POWER_DOWN_EANBLE
	unsigned char  temp[5];
#endif

	typedef enum
	{
		First_PowerOn_Stage=0,			
		First_Calibration_Setting=0x01,
		First_Wait_2mS_Delay = 0x02,
		First_CE_Pulse_Opt = 0x03,
		First_Wait_40mS_Delay = 0x04,
		First_RF_Config_Stage= 0x05,
		First_RF_Turn_ON = 0x06,
		ReInit_Calibration_Setting = 0x80,
		ReInit_CE_Pulse_Opt = 0x83,		//Must more than 2mS delay @last status;
		ReInit_RF_Config_Stage  = 0xA0,	//Must more than 40mS delay @last status;
		ReInit_RF_Turn_ON = 0xA1		//End of Init stage
	}RF_InitStatusType;

	static RF_InitStatusType InitFlag = First_PowerOn_Stage;

	while(1)
	{
		switch(InitFlag)
		{			
			case First_PowerOn_Stage:// ID checking!
				while(RF_Get_Chip_ID() != 0x01){}
				InitFlag=First_Calibration_Setting;
				break;

			case First_Calibration_Setting:
			case ReInit_Calibration_Setting:
				RF_Soft_Rst();				//Make Sure RF reset SYNC MCU
				RF_Bank_Switch(RF_Bank0);/*switch to bank1 for Calibration Init*/	
				RF_CE_Low();
				RF_Configure_Reg(RF_Calibration_Data);
				RF_Bank_Switch(RF_Bank1);/*switch to bank1 for Calibration Init*/	
				RF_Configure_Reg(RF_Calibration_Analog);	
				RF_Bank_Switch(RF_Bank0);
				InitFlag++;
				break;

			case First_Wait_2mS_Delay:
				Delay1ms(2);//delay 2ms
				InitFlag = First_CE_Pulse_Opt;
				break;

			case First_CE_Pulse_Opt:
			case ReInit_CE_Pulse_Opt:
				RF_CE_High();
				Delay1ms(2); //greater than 2us
				RF_CE_Low();
				InitFlag++;
				break;

			case First_Wait_40mS_Delay:
				Delay1ms(40);//delay 40ms
				InitFlag = First_RF_Config_Stage;
				break;

			case First_RF_Config_Stage:
			case ReInit_RF_Config_Stage:
				RF_Bank_Switch(RF_Bank1);
				RF_Configure_Reg(RF_Init_Analog);
				RF_Bank_Switch(RF_Bank0);;
				RF_Configure_Reg(RF_Init_Data);
				InitFlag++;
				break;

			case First_RF_Turn_ON:
			case ReInit_RF_Turn_ON:				
				RF_Clear_All_Irq();
				//HS6206_ChangeAddr_Reg(BIND_Address,5);//BIND_Address is the communication address array, global variable
				RF_Flush_Tx();
				RF_Flush_Rx();

#ifdef HS6206_POWER_DOWN_EANBLE
				/*read and save reg for powerdown without init*/
				RF_CE_Low();

				RF_Bank_Switch(RF_Bank1);

				RF_read_buffer(RF_BANK1_CAL_CTL, temp, 5);
				temp[0] |=0xc0;
				RF_wr_buffer(RF_BANK1_CAL_CTL, temp, 5);

				RF_read_buffer(RF_BANK1_PLL_CTL0, temp, 5);
				temp[0] |=0x20;
				RF_wr_buffer(RF_BANK1_PLL_CTL0, temp, 5);

				RF_read_buffer(RF_BANK1_DAC_RANGE, temp, 1);

				RF_wr_buffer(RF_BANK1_DAC_RANGE, temp, 1);

				RF_Bank_Switch(RF_Bank0);
#endif

				RF_CE_High();

				InitFlag = ReInit_Calibration_Setting;
				return 0xFF;

			default://other delay split, once more than 1.4mS
				InitFlag++;
				return 0;
		}
	}
}
#endif


#if 0
void RF_Dump_RF_Register(void)
{
	XDATA unsigned char  reg_value;
	XDATA unsigned char  temp[5]; 
	XDATA signed char i;

	RF_Bank_Switch(RF_Bank0);

	RF_read_buffer(RF_BANK0_CONFIG, &reg_value, 1);
	printf("RF_BANK0_CONFIG: %02x\r\n", (int)reg_value);  

	RF_read_buffer(RF_BANK0_EN_AA, &reg_value, 1);
	printf("RF_BANK0_EN_AA: %02x\r\n", (int)reg_value); 

	RF_read_buffer(RF_BANK0_EN_RXADDR, &reg_value, 1);
	printf("RF_BANK0_EN_RXADDR: %02x\r\n", (int)reg_value); 

	RF_read_buffer(RF_BANK0_SETUP_AW, &reg_value, 1);
	printf("RF_BANK0_SETUP_AW: %02x\r\n", (int)reg_value); 

	RF_read_buffer(RF_BANK0_SETUP_RETR, &reg_value, 1);
	printf("RF_BANK0_SETUP_RETR: %02x\r\n", (int)reg_value);  

	RF_read_buffer(RF_BANK0_RF_CH, &reg_value, 1);
	printf("RF_BANK0_RF_CH: %02x\r\n", (int)reg_value); 

	RF_read_buffer(RF_BANK0_RF_SETUP, &reg_value, 1);
	printf("RF_BANK0_RF_SETUP: %02x\r\n", (int)reg_value); 

	RF_read_buffer(RF_BANK0_STATUS, &reg_value, 1);
	printf("RF_BANK0_STATUS: %02x\r\n", (int)reg_value); 

	RF_read_buffer(RF_BANK0_OBSERVE_TX, &reg_value, 1);
	printf("RF_BANK0_OBSERVE_TX: %02x\r\n", (int)reg_value); 

	RF_read_buffer(RF_BANK0_RPD, &reg_value, 1);
	printf("RF_BANK0_RPD: %02x\r\n", (int)reg_value); 

	RF_read_buffer(RF_BANK0_RX_ADDR_P0, temp, 5);
	printf("RF_BANK0_RX_ADDR_P0: "); 
	for(i=4;i>=0;i--)
		printf("%02x " , (int)temp[i]);
	printf("\r\n");

	RF_read_buffer(RF_BANK0_RX_ADDR_P1, &reg_value, 1);
	printf("RF_BANK0_RX_ADDR_P1: %02x\r\n", (int)reg_value);  

	RF_read_buffer(RF_BANK0_RX_ADDR_P2, &reg_value, 1);
	printf("RF_BANK0_RX_ADDR_P2: %02x\r\n", (int)reg_value); 

	RF_read_buffer(RF_BANK0_RX_ADDR_P3, &reg_value, 1);
	printf("RF_BANK0_RX_ADDR_P3: %02x\r\n", (int)reg_value); 

	RF_read_buffer(RF_BANK0_RX_ADDR_P4, &reg_value, 1);
	printf("RF_BANK0_RX_ADDR_P4: %02x\r\n", (int)reg_value);

	RF_read_buffer(RF_BANK0_RX_ADDR_P5, &reg_value, 1);
	printf("RF_BANK0_RX_ADDR_P5: %02x\r\n", (int)reg_value);  

	RF_read_buffer(RF_BANK0_TX_ADDR, temp, 5);
	printf("RF_BANK0_TX_ADDR: "); 
	for(i=4;i>=0;i--)
		printf("%02x " , (int)temp[i]);
	printf("\r\n");

	RF_read_buffer(RF_BANK0_RX_PW_P0, &reg_value, 1);
	printf("RF_BANK0_RX_PW_P0: %02x\r\n", (int)reg_value); 

	RF_read_buffer(RF_BANK0_RX_PW_P1, &reg_value, 1);
	printf("RF_BANK0_RX_PW_P1: %02x\r\n", (int)reg_value); 

	RF_read_buffer(RF_BANK0_RX_PW_P2, &reg_value, 1);
	printf("RF_BANK0_RX_PW_P2: %02x\r\n", (int)reg_value);  

	RF_read_buffer(RF_BANK0_RX_PW_P3, &reg_value, 1);
	printf("RF_BANK0_RX_PW_P3: %02x\r\n", (int)reg_value); 

	RF_read_buffer(RF_BANK0_RX_PW_P4, &reg_value, 1);
	printf("RF_BANK0_RX_PW_P4: %02x\r\n", (int)reg_value); 

	RF_read_buffer(RF_BANK0_RX_PW_P5, &reg_value, 1);
	printf("RF_BANK0_RX_PW_P5: %02x\r\n", (int)reg_value); 

	RF_read_buffer(RF_BANK0_FIFO_STATUS, &reg_value, 1);
	printf("RF_BANK0_FIFO_STATUS: %02x\r\n", (int)reg_value);  

	RF_read_buffer(RF_BANK0_DYNPD, &reg_value, 1);
	printf("RF_BANK0_DYNPD: %02x\r\n", (int)reg_value); 

	RF_read_buffer(RF_BANK0_FEATURE, &reg_value, 1);
	printf("RF_BANK0_FEATURE: %02x\r\n", (int)reg_value); 


	RF_read_buffer(RF_BANK0_SETUP_VALUE, temp, 5);
	printf("RF_BANK0_SETUP_VALUE: ");
	for(i=4;i>=0;i--)
		printf("%02x " , (int)temp[i]);
	printf("\r\n");


	RF_read_buffer(RF_BANK0_PRE_GURD, &reg_value, 1);
	printf("RF_BANK0_PRE_GURD: %02x\r\n\n\n", (int)reg_value);  

	RF_Bank_Switch(RF_Bank1);

	RF_read_buffer(RF_BANK1_LINE, temp, 2);
	printf("RF_BANK1_LINE: ");
	for(i=1;i>=0;i--)
		printf("%02x " , (int)temp[i]);
	printf("\r\n"); 

	RF_read_buffer(RF_BANK1_PLL_CTL0, temp, 4);
	printf("RF_BANK1_PLL_CTL0: ");
	for(i=3;i>=0;i--)
		printf("%02x " , (int)temp[i]);
	printf("\r\n");  

	RF_read_buffer(RF_BANK1_PLL_CTL1, temp, 4);
	printf("RF_BANK1_PLL_CTL1: ");
	for(i=3;i>=0;i--)
		printf("%02x " , (int)temp[i]);
	printf("\r\n");

	RF_read_buffer(RF_BANK1_CAL_CTL, temp, 5);
	printf("RF_BANK1_CAL_CTL: ");
	for(i=4;i>=0;i--)
		printf("%02x " , (int)temp[i]);
	printf("\r\n");

	RF_read_buffer(RF_BANK1_A_CNT_REG, &reg_value, 1);
	printf("RF_BANK1_A_CNT_REG: %02x\r\n", (int)reg_value);   

	RF_read_buffer(RF_BANK1_B_CNT_REG, &reg_value, 1);
	printf("RF_BANK1_B_CNT_REG: %02x\r\n", (int)reg_value);  

	RF_read_buffer(RF_BANK1_RESERVED1, &reg_value, 1);
	printf("RF_BANK1_RESERVED1: %02x\r\n", (int)reg_value);  

	RF_read_buffer(RF_BANK1_STATUS, &reg_value, 1);
	printf("RF_BANK1_STATUS: %02x\r\n", (int)reg_value);  

	RF_read_buffer(RF_BANK1_STATE, temp, 2);
	printf("RF_BANK1_STATE: ");
	for(i=1;i>=0;i--)
		printf("%02x " , (int)temp[i]);
	printf("\r\n");  

	RF_read_buffer(RF_BANK1_CHAN, temp, 4);
	printf("RF_BANK1_CHAN: ");
	for(i=3;i>=0;i--)
		printf("%02x " , (int)temp[i]);
	printf("\r\n");  

	RF_read_buffer(RF_BANK1_IF_FREQ, temp, 3);
	printf("RF_BANK1_IF_FREQ: ");
	for(i=2;i>=0;i--)
		printf("%02x " , (int)temp[i]);
	printf("\r\n");

	RF_read_buffer(RF_BANK1_AFC_COR, temp, 3);
	printf("RF_BANK1_AFC_COR: ");
	for(i=2;i>=0;i--)
		printf("%02x " , (int)temp[i]);
	printf("\r\n");

	RF_read_buffer(RF_BANK1_FDEV, &reg_value, 1);
	printf("RF_BANK1_FDEV: %02x\r\n", (int)reg_value);   

	RF_read_buffer(RF_BANK1_DAC_RANGE, &reg_value, 1);
	printf("RF_BANK1_DAC_RANGE: %02x\r\n", (int)reg_value);  

	RF_read_buffer(RF_BANK1_DAC_IN, &reg_value, 1);
	printf("RF_BANK1_DAC_IN: %02x\r\n", (int)reg_value);

	RF_read_buffer(RF_BANK1_CTUNING, temp, 2);
	printf("RF_BANK1_CTUNING: ");
	for(i=1;i>=0;i--)
		printf("%02x " , (int)temp[i]);
	printf("\r\n"); 

	RF_read_buffer(RF_BANK1_FTUNING, temp, 2);
	printf("RF_BANK1_FTUNING: ");
	for(i=1;i>=0;i--)
		printf("%02x " , (int)temp[i]);
	printf("\r\n"); 

	RF_read_buffer(RF_BANK1_RX_CTRL, temp, 4);
	printf("RF_BANK1_RX_CTRL: ");
	for(i=3;i>=0;i--)
		printf("%02x " , (int)temp[i]);
	printf("\r\n");  

	RF_read_buffer(RF_BANK1_FAGC_CTRL, temp, 4);
	printf("RF_BANK1_FAGC_CTRL: ");
	for(i=3;i>=0;i--)
		printf("%02x " , (int)temp[i]);
	printf("\r\n");

	RF_read_buffer(RF_BANK1_FAGC_CTRL_1, temp, 4);
	printf("RF_BANK1_FAGC_CTRL_1: ");
	for(i=3;i>=0;i--)
		printf("%02x " , (int)temp[i]);
	printf("\r\n");

	RF_read_buffer(RF_BANK1_DAC_CAL_LOW, &reg_value, 1);
	printf("RF_BANK1_DAC_CAL_LOW: %02x\r\n", (int)reg_value);   

	RF_read_buffer(RF_BANK1_DAC_CAL_HI, &reg_value, 1);
	printf("RF_BANK1_DAC_CAL_HI: %02x\r\n", (int)reg_value);  

	RF_read_buffer(RF_BANK1_RESERVED3, &reg_value, 1);
	printf("RF_BANK1_RESERVED3: %02x\r\n", (int)reg_value);  

	RF_read_buffer(RF_BANK1_DOC_DACI, &reg_value, 1);
	printf("RF_BANK1_DOC_DACI: %02x\r\n", (int)reg_value);  

	RF_read_buffer(RF_BANK1_DOC_DACQ, &reg_value, 1);
	printf("RF_BANK1_DOC_DACQ: %02x\r\n", (int)reg_value);   

	RF_read_buffer(RF_BANK1_AGC_CTRL, temp, 4);
	printf("RF_BANK1_AGC_CTRL: ");
	for(i=3;i>=0;i--)
		printf("%02x " , (int)temp[i]);
	printf("\r\n"); 

	RF_read_buffer(RF_BANK1_AGC_GAIN, temp, 4);
	printf("RF_BANK1_AGC_GAIN: ");
	for(i=3;i>=0;i--)
		printf("%02x " , (int)temp[i]);
	printf("\r\n");  

	RF_read_buffer(RF_BANK1_RF_IVGEN, temp, 4);
	printf("RF_BANK1_RF_IVGEN: ");
	for(i=3;i>=0;i--)
		printf("%02x " , (int)temp[i]);
	printf("\r\n");

	RF_read_buffer(RF_BANK1_TEST_PKDET, temp, 4);
	printf("RF_BANK1_TEST_PKDET: ");
	for(i=3;i>=0;i--)
		printf("%02x " , (int)temp[i]);
	printf("\r\n");  
	RF_Bank_Switch(RF_Bank0);
}
#endif

/*-------------------------------------------End Of File---------------------------------------------*/
