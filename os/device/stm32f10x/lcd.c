#include <common.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>
#include <lcd.h>

/* PB0: RST */
#define LCD_GPIO1	GPIOB
#define LCD_PIN_RST	GPIO_Pin_0
/* PB1: A0*/
#define LCD_PIN_A0	GPIO_Pin_1
/* PA4: CS */
#define LCD_GPIO2	GPIOA
#define LCD_PIN_CS	GPIO_Pin_4
/* PA5: SCLK */
#define LCD_PIN_SCLK	GPIO_Pin_5
/* PA6: MISO */
#define LCD_PIN_MISO	GPIO_Pin_6
/* PA7: MOSI */
#define LCD_PIN_MOSI	GPIO_Pin_7

#define LCD_SPI	SPI1

void lcd_pin_init(void)
{
	GPIO_InitTypeDef Init;

	//SPI in master mode
	// enable peripherial clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	// RST
	Init.GPIO_Pin	= LCD_PIN_RST;
	Init.GPIO_Mode	= GPIO_Mode_Out_PP;
	Init.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(LCD_GPIO1,&Init);
	//GPIO_WriteBit(LCD_GPIO1,LCD_PIN_RST,Bit_SET);//rst = 1
	// A0
	Init.GPIO_Pin	= LCD_PIN_A0;
	Init.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_Init(LCD_GPIO1,&Init);
	// CS
	Init.GPIO_Pin	= LCD_PIN_CS;
	Init.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_Init(LCD_GPIO2,&Init);
	// SCLK
	Init.GPIO_Pin	= LCD_PIN_SCLK;
	Init.GPIO_Mode	= GPIO_Mode_AF_PP;
	GPIO_Init(LCD_GPIO2,&Init);
	// MOSI
	Init.GPIO_Pin	= LCD_PIN_MOSI;
	Init.GPIO_Mode	= GPIO_Mode_AF_PP;
	GPIO_Init(LCD_GPIO2,&Init);
	// MISO
	Init.GPIO_Pin	= LCD_PIN_MISO;
	Init.GPIO_Mode	= GPIO_Mode_IN_FLOATING;
	GPIO_Init(LCD_GPIO2,&Init);
}

#if 0
void lcd_spi_init(void)
{
	SPI_InitTypeDef Init;

	//enable SPI1 clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	SPI_I2S_DeInit(LCD_SPI);
	
	Init.SPI_Direction		= SPI_Direction_2Lines_FullDuplex;
	Init.SPI_Mode			= SPI_Mode_Master;
	Init.SPI_DataSize		= SPI_DataSize_8b;
	Init.SPI_CPOL			= SPI_CPOL_Low;
	Init.SPI_CPHA			= SPI_CPHA_1Edge;
	Init.SPI_NSS			= SPI_NSS_Hard;
	Init.SPI_BaudRatePrescaler	= SPI_BaudRatePrescaler_32;
	Init.SPI_CRCPolynomial		= 7;
	SPI_Init(LCD_SPI,&Init);

	SPI_I2S_ITConfig(LCD_SPI,SPI_I2S_IT_RXNE,DISABLE);
	SPI_I2S_ITConfig(LCD_SPI,SPI_I2S_IT_TXE,DISABLE);

	LCD_SPI->CR2 = 0x0004;//ss output disable

	SPI_Cmd(LCD_SPI,ENABLE);
}
#else
void lcd_spi_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	SPI_I2S_DeInit(LCD_SPI);

	//disabe spi
	LCD_SPI->CR1 &= ~0x0040;

	//full duplex
	// [9]   SSM = 1
	// [7]   0: MSB, 1: LSB
	// [6]   0: disable, 1: enable
	// [5:3] 111: BR = Fpclk / 128
	// [2]   1: Master, 0: Slave
	// [1] 	 CPOL = 1 Idle clk = 1
	// [0]   CPHA = 1 2th edge sampling
	// 0000 0010 0001 1111
	LCD_SPI->CR1 = 0x023F;

	//active the spi mode
	LCD_SPI->I2SCFGR &= 0xF7FF;
	//write CRCPOLY
	LCD_SPI->CRCPR = 0x07;
	//enable ss output
	LCD_SPI->CR2 = 0x0004;
	//enabe spi
	LCD_SPI->CR1 |= 0x0040;
}

#endif
void lcd_set_cs(int level)
{
	if(level)
		GPIO_WriteBit(LCD_GPIO2,LCD_PIN_CS,Bit_SET);//cs = 1
	else
		GPIO_WriteBit(LCD_GPIO2,LCD_PIN_CS,Bit_RESET);//cs = 0
}

/* if cmd = 1, set A0 = low level */
void lcd_set_a0(int level)
{
	if(level)
		GPIO_WriteBit(LCD_GPIO2,LCD_PIN_A0,Bit_SET);//A0 = 1
	else
		GPIO_WriteBit(LCD_GPIO2,LCD_PIN_A0,Bit_RESET);//A0 = 0
}

void lcd_set_rst(int level)
{
	if(level)
		GPIO_WriteBit(LCD_GPIO1,LCD_PIN_RST,Bit_SET);
	else
		GPIO_WriteBit(LCD_GPIO1,LCD_PIN_RST,Bit_RESET);
}

enum LCD_CMD_TYPE
{
	LCD_DAT = 0,
	LCD_CMD,
};

static void lcd_spi_write(uint8_t dat, enum LCD_CMD_TYPE cmd)
{
	uint8_t dummy;

	// set A0 line
	if(cmd == LCD_CMD)
		lcd_set_a0(0);
	else
		lcd_set_a0(1);

	//wait until txfifo is emtpy
	while((LCD_SPI->SR & SPI_I2S_FLAG_TXE) == 0);

	//set cs = 0
	lcd_set_cs(0);

	// write tx buffer
	LCD_SPI->DR = dat;

	//wait until rxfifo is not empty
	while((LCD_SPI->SR & SPI_I2S_FLAG_RXNE) == 0);

	// read rx buffer
	dummy = LCD_SPI->DR;
	dummy = dummy;

	//set cs = 1
	//udelay(10);
	lcd_set_cs(1);
#if 0
	if(cmd)
		rprintf("spi: tx: %2x, rx: %2x, cmd: %2x\r\n",dat,dummy,cmd);
#endif
}

#define LCD_CMD_END	0xEE
#define LCD_CMD_RESET	0xE2
#define LCD_CMD_APON	0xA5
#define LCD_CMD_APOFF	0xA4
#define LCD_CMD_BIAS	0xA2

#define LCD_CMD_PAGE	0xB0
#define LCD_CMD_SLINE	0x40
#define LCD_CMD_COL_ADDRH	0x10
#define LCD_CMD_COL_ADDRL	0x01
#define LCD_CMD_NOP		0xE3

uint8_t lcd_pages[] = {0,1,2,3,4,5,6,7};

void lcd_screen_clear(uint8_t fill)
{
	int i,j;

	for(i = 0; i < LCD_PAGE_NR;i++) {
		lcd_spi_write(LCD_CMD_PAGE | i,  LCD_CMD);
		lcd_spi_write(LCD_CMD_COL_ADDRH, LCD_CMD);
		lcd_spi_write(LCD_CMD_COL_ADDRL, LCD_CMD);

		for(j = 0;j < LCD_COL_DOT_NR;j++) {
			lcd_spi_write(fill, LCD_DAT);
		}
	}
}

void lcd_screen_init(void)
{
	mdelay(10);

	lcd_set_rst(0);
	mdelay(200);

	lcd_set_rst(1);
	mdelay(50);

	lcd_spi_write(0xAE, LCD_CMD); //display OFF
	lcd_spi_write(0xA2, LCD_CMD); //bias, 1/9
	lcd_spi_write(0xA0, LCD_CMD); //ADC select
	lcd_spi_write(0xC0, LCD_CMD); //com1->com64
	lcd_spi_write(0x24, LCD_CMD); //Rb/Ra

	lcd_spi_write(0x81, LCD_CMD); //contrast
	lcd_spi_write(50,   LCD_CMD);

	lcd_spi_write(0x2F, LCD_CMD); //Voltage
	lcd_spi_write(0xA6, LCD_CMD); //Normal display
	lcd_spi_write(0xA4, LCD_CMD); //All pixel off
	lcd_spi_write(0x40, LCD_CMD); //Display start line = 0
	lcd_spi_write(0xB0, LCD_CMD); //Page address = 0
	lcd_spi_write(0x10, LCD_CMD); //Column address high
	lcd_spi_write(0x01, LCD_CMD); //column address low
	lcd_screen_clear(0x0);
	lcd_spi_write(0xAF, LCD_CMD); //Display ON
	//lcd_spi_write(0xA5, LCD_CMD); //All pixel ON
}

void lcd_init(void)
{
	lcd_pin_init();
	lcd_spi_init();
	lcd_set_cs(1);
	lcd_set_a0(0);
	lcd_screen_init();
}

extern unsigned char lcd_asclib8x16[8][16];
extern unsigned char lcd_disp[16];

void lcd_screen_putc(int page, int col, uint8_t ch)
{
	int i = 0;
//	int j = 7;
	int idx = 0;

	//first page
	lcd_spi_write(0xB0 | lcd_pages[page & 0x7], LCD_CMD);
	lcd_spi_write(((col + 1) >> 4) | 0x10, LCD_CMD); //set col high
	lcd_spi_write(((col + 1) >> 0) & 0x0F, LCD_CMD); //set col low

	for(i = 0;i < 8;i++)
		lcd_spi_write(lcd_disp[idx++],LCD_DAT);

	//next page
	lcd_spi_write(0xB0 | lcd_pages[(page + 1) & 0x7], LCD_CMD);
	lcd_spi_write(((col + 1) >> 4) | 0x10, LCD_CMD); //set col high
	lcd_spi_write(((col + 1) >> 0) & 0x0F, LCD_CMD); //set col low

	for(i = 0;i < 8;i++)
		lcd_spi_write(lcd_disp[idx++],LCD_DAT);

}

void lcd_screen_puts(int row, int col, const char *s)
{

}

void lcd_test(void)
{
	rprintf("%s, start\r\n",__func__);
	lcd_init();

//	lcd_screen_putc(0,10,'A');
//	lcd_screen_putc(1,10,'A');
//	lcd_screen_putc(3,10,'A');
	lcd_screen_putc(4,10,'A');
	while(1) {};
}

