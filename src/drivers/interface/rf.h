//------------------------------------------------------------------------------------
// HS6206.h
//------------------------------------------------------------------------------------
// Copyright 2014, daxiazhuluoji
// Yufeng. Yao
// 2014-01-16
//
// Program Description: HS6200 command, register and 
//
//
//


#ifndef __RF_H__
#define __RF_H__

#define MAX_RT    0x10
#define TX_DS   	0x20
#define RX_DR   	0x40

#define RF_CLK_EN             BIT2                 /*RF clock*/
#define RFSPI_CSN             BIT1                 /*Enable RF command. 0: enabled*/
#define RFSPI_CE              BIT0                 /*Enable RF transceiver. 1: enabled*/

//----------------SPIRCON--------------
#define RFSPI_MASK_RX_FIFO_FULL_INT   BIT3          /*Disable interrupt when RX FIFO is full.*/    
#define RFSPI_MASK_RX_DATA_RDY_INT    BIT2          /*Disable interrupt when data is available in RX FIFO*/
#define RFSPI_MASK_TX_FIFO_EMPTY_INT  BIT1          /*Disable interrupt when TX FIFO is empty.*/
#define RFSPI_MASK_TX_FIFO_RDY_INT    BIT0          /*Disable interrupt when a location is available in TX FIFO*/

//----------------SPIRSTAT-----------------
#define RFSPI_RX_FIFO_FULL           BIT3           /*RX FIFO full.*/ 
#define RFSPI_RX_DATA_RDY            BIT2           /*Data available in RX FIFO*/
#define RFSPI_TX_FIFO_EMPTY          BIT1           /*TX FIFO empty.*/
#define RFSPI_TX_FIFO_RDY            BIT0           /*Location available in TX FIFO*/

#define RF_STATUS_RX_DR				0x40
#define RF_STATUS_TX_DS				0x20
#define RF_STATUS_MAX_RT			0x10
#define RF_STATUS_TX_FULL			0x01


//-------------RF command---------------------
#define RF_R_REGISTER                 0x00 // Define read command to register
#define RF_W_REGISTER                 0x20 // Define write command to register
#define RF_R_RX_PAYLOAD               0x61 // Define RX payload register address
#define RF_W_TX_PAYLOAD               0xA0 // Define TX payload register address
#define RF_FLUSH_TX                   0xE1 // Define flush TX register command
#define RF_FLUSH_RX                   0xE2 // Define flush RX register command
#define RF_REUSE_TX_PL                0xE3 // Define reuse TX payload register command
#define RF_ACTIVATE                   0x50 // Define ACTIVATE features register command
#define RF_R_RX_PL_WID                0x60 // Define read RX payload width register command
#define RF_W_ACK_PAYLOAD              0xA8 // Define write ACK payload register command
#define RF_W_TX_PAYLOAD_NOACK         0xB0 // Define disable TX ACK for one time register command
#define RF_NOP                        0xFF // Define No Operation, might be used to read status register
#define RF_READ_STATUS                0xFF

#define RF_ACTIVATE_DATA              0x53


//----------------------defines-------------------------- 
/*
 * 对HS6206的寄存器进行扩充，BANK1寄存器的最高位为1，而BANK0为实际地址。 
 * 
 */
#define BIT7 0x80
#define RF_BANK_BIT    BIT7      /*bank 指示位*/
#define RF_BANK1       BIT7
#define RF_BANK0       0x00  

//HS6206 Bank0 Register Addr
/**************************************************/               
#define RF_BANK0_CONFIG                     0x00 // 'Config' register address
#define RF_BANK0_EN_AA                      0x01 // 'Enable Auto Acknowledgment' register address
#define RF_BANK0_EN_RXADDR                  0x02 // 'Enabled RX addresses' register address
#define RF_BANK0_SETUP_AW                   0x03 // 'Setup address width' register address
#define RF_BANK0_SETUP_RETR                 0x04 // 'Setup Auto. Retrans' register address
#define RF_BANK0_RF_CH                      0x05 // 'RF channel' register address
#define RF_BANK0_RF_SETUP                   0x06 // 'RF setup' register address
#define RF_BANK0_STATUS                     0x07 // 'Status' register address
#define RF_BANK0_OBSERVE_TX                 0x08 // 'Observe TX' register address
#define RF_BANK0_RPD                        0x09 // 'Received Power Detector' register address
#define RF_BANK0_RX_ADDR_P0                 0x0A // 'RX address pipe0' register address
#define RF_BANK0_RX_ADDR_P1                 0x0B // 'RX address pipe1' register address
#define RF_BANK0_RX_ADDR_P2                 0x0C // 'RX address pipe2' register address
#define RF_BANK0_RX_ADDR_P3                 0x0D // 'RX address pipe3' register address
#define RF_BANK0_RX_ADDR_P4                 0x0E // 'RX address pipe4' register address
#define RF_BANK0_RX_ADDR_P5                 0x0F // 'RX address pipe5' register address
#define RF_BANK0_TX_ADDR                    0x10 // 'TX address' register address
#define RF_BANK0_RX_PW_P0                   0x11 // 'RX payload width, pipe0' register address
#define RF_BANK0_RX_PW_P1                   0x12 // 'RX payload width, pipe1' register address
#define RF_BANK0_RX_PW_P2                   0x13 // 'RX payload width, pipe2' register address
#define RF_BANK0_RX_PW_P3                   0x14 // 'RX payload width, pipe3' register address
#define RF_BANK0_RX_PW_P4                   0x15 // 'RX payload width, pipe4' register address
#define RF_BANK0_RX_PW_P5                   0x16 // 'RX payload width, pipe5' register address
#define RF_BANK0_FIFO_STATUS                0x17 // 'FIFO Status Register' register address
#define RF_BANK0_DYNPD                      0x1C // 'Enable dynamic payload length' register address
#define RF_BANK0_FEATURE                    0x1D // 'Feature' register address
#define RF_BANK0_SETUP_VALUE                0x1E
#define RF_BANK0_PRE_GURD                   0x1F


//data rate
#define RF_BANK0_DR         (BIT5+BIT3) 
#define RF_BANK0_DR_250K     BIT5  
#define RF_BANK0_DR_500K	(BIT5+BIT3)
#define RF_BANK0_DR_1M		 0x00
#define RF_BANK0_DR_2M	     BIT3

//HS6206_BANK0_SETUP_AW
#define RF_AW_BITS     (BIT0+BIT1)
#define RF_AW_4_BYTES  BIT1
#define RF_AW_5_BYTES  (BIT0+BIT1)

/*------------------------------5.PA power-----------------------------------*/
#define RF_BANK0_PA_POWER              (BIT2+BIT1+BIT0) 
#define RF_BANK0_PA_POWER_n18dBm        0x00
#define RF_BANK0_PA_POWER_n12dBm        BIT1
#define RF_BANK0_PA_POWER_n6dBm        	BIT2
#define RF_BANK0_PA_POWER_0dBm        	(BIT2+BIT1)
#define RF_BANK0_PA_POWER_5dBm          BIT0  

//Cont wave
#define RF_BANK0_CONT_WAVE                BIT7  
#define RF_BANK0_PRX                      BIT0


//HS6206 Bank1 register
#define RF_BANK1_LINE                 0x00
#define RF_BANK1_PLL_CTL0             0x01
#define RF_BANK1_PLL_CTL1             0x02
#define RF_BANK1_CAL_CTL              0x03
#define RF_BANK1_A_CNT_REG            0x04
#define RF_BANK1_B_CNT_REG            0x05
#define RF_BANK1_RESERVED1            0x06
#define RF_BANK1_STATUS               0x07
#define RF_BANK1_STATE                0x08
#define RF_BANK1_CHAN                 0x09
#define RF_BANK1_IF_FREQ              0x0A
#define RF_BANK1_AFC_COR              0x0B
#define RF_BANK1_FDEV                 0x0C
#define RF_BANK1_DAC_RANGE            0x0D
#define RF_BANK1_DAC_IN               0x0E
#define RF_BANK1_CTUNING              0x0F
#define RF_BANK1_FTUNING              0x10
#define RF_BANK1_RX_CTRL              0x11
#define RF_BANK1_FAGC_CTRL            0x12
#define RF_BANK1_FAGC_CTRL_1          0x13
#define RF_BANK1_DAC_CAL_LOW          0x17
#define RF_BANK1_DAC_CAL_HI           0x18
#define RF_BANK1_RESERVED3            0x19
#define RF_BANK1_DOC_DACI             0x1A
#define RF_BANK1_DOC_DACQ             0x1B
#define RF_BANK1_AGC_CTRL             0x1C
#define RF_BANK1_AGC_GAIN             0x1D
#define RF_BANK1_RF_IVGEN             0x1E
#define RF_BANK1_TEST_PKDET           0x1F

#define RF_FIFO_MAX_PACK_SIZE		0x20

/*register bit Mask define*/
/*R 0x17:*/
#define RF_FIFO_STA_TX_REUSE		0x40
#define RF_FIFO_STA_TX_FULL			0x20
#define RF_FIFO_STA_TX_EMPTY		0x10
#define RF_FIFO_STA_RX_FULL			0x02
#define RF_FIFO_STA_RX_EMPTY		0x01


typedef enum {
	Rf_PRX_Mode = 0,
	Rf_PTX_Mode,
	Rf_Carrier_Mode
}RF_ModeTypeDef;

typedef enum {
	RF_Bank0 = 0,
	RF_Bank1
}RF_Bank_TypeDef;


extern void RF_Bank_Switch(RF_Bank_TypeDef bank);
extern void RF_CE_High(void);
extern void RF_CE_High_Pulse(void);
extern void RF_CE_Low(void);
extern void RF_ChangeAddr_Reg(unsigned char* AddrBuf,unsigned char len);
extern void RF_Change_CH(unsigned char ch_index);
extern void RF_Change_Pwr(signed char Pwr_dBm);
extern void RF_Clear_All_Irq(void);
extern void RF_Configure_Reg(const unsigned char* Dbuf);
extern void RF_Flush_Rx(void);
extern void RF_Flush_Tx(void);
extern unsigned char RF_Get_Chip_ID(void);
extern signed char RF_Get_RSSI(void);
extern void RF_ModeSwitch(RF_ModeTypeDef mod);
extern unsigned char RF_Operation(unsigned char opt);
extern void RF_read_buffer(unsigned char addr, unsigned char* buf, unsigned char len);
extern unsigned char RF_read_byte(unsigned char addr);
extern unsigned char RF_Read_Status(void);
extern unsigned char RF_ReceivePack(unsigned char* buf);
extern void RF_SendPack(unsigned char cmd, unsigned char* buf, unsigned char len);
extern void RF_Soft_Rst(void);
extern unsigned char RF_spi_wrd(unsigned char Byte);
extern void RF_Write_Ack_Payload(unsigned char PipeNum, unsigned char *pBuf, unsigned char bytes);
extern void RF_wr_buffer(unsigned char addr,const unsigned char* buf,unsigned char len);
extern void RF_write_byte(unsigned char addr,unsigned char D);
extern void RF_wr_cmd(unsigned char cmd,unsigned char D);
extern void RF_Init(void);
//extern unsigned char RF_Init(void);

extern void RF_Dump_RF_Register(void);

#endif  /*__HS6206_RF_REG_H__*/

/*-------------------------------------------End Of File---------------------------------------------*/
