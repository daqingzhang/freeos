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
	GPIO_WriteBit(LCD_GPIO1,LCD_PIN_RST,Bit_RESET);//rst = 0
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

	//disabe spi to config
	LCD_SPI->CR1 &= ~0x0040;
	// master, LSB, full duplex, CPOL = 0, CPHA = 1th edge,
	// 0000 0010 1011 1100
	LCD_SPI->CR1 = 0x02BC;
	// set prescaler = 128, val = 0x7
	LCD_SPI->CR1 &= ~(0x7 << 3);
	LCD_SPI->CR1 |= (0x7 << 3);
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

void lcd_reset(void)
{
	unsigned int pin = LCD_PIN_RST;

	GPIO_WriteBit(LCD_GPIO1,pin,Bit_SET);
	mdelay(5);

	GPIO_WriteBit(LCD_GPIO1,pin,Bit_RESET);
	mdelay(5);
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
	lcd_set_cs(1);

	// write tx buffer
	LCD_SPI->DR = dat;

	//wait until rxfifo is not empty
	while((LCD_SPI->SR & SPI_I2S_FLAG_RXNE) == 0);

	// read rx buffer
	dummy = LCD_SPI->DR;
	dummy = dummy;

	//set cs = 1
	udelay(10);
	lcd_set_cs(0);

#if 1
	rprintf("%s, tx: %2x, rx: %2x, cmd: %2x\r\n",__func__,dat,dummy,cmd);
#endif
}

uint8_t lcd_pages[] = {3,2,1,0,7,6,5,4};

void lcd_screen_clear(uint8_t fill)
{
	int i,j;

	for(i = 0; i < LCD_ROW_DATA_NR;i++) {
		lcd_spi_write(0xB0 | lcd_pages[i], LCD_CMD);
		lcd_spi_write(0x10, LCD_CMD);//column addr = 0
		lcd_spi_write(0x01, LCD_CMD);//column auto add

		for(j = 0;j < LCD_COL_DATA_END;j++)
			lcd_spi_write(fill, LCD_DAT);
	}
}

#define LCD_CMD_END	0xEE
#define LCD_CMD_RESET	0xE2
#define LCD_CMD_APON	0xA5
#define LCD_CMD_APON	0xA4
#define LCD_CMD_BIAS	0xA2
#if 0
void lcd_screen_init(void)
{
	//display OFF
	lcd_spi_write(0xAE, LCD_CMD);

	//SEG direction, normal
	lcd_spi_write(0xA0, LCD_CMD);

	//Inverse display,normal
	lcd_spi_write(0xA6,LCD_CMD);

	//All pixel ON, OFF: bit[0] = 0; ON: bit[0] = 1;
	lcd_spi_write(0xA4,LCD_CMD);

	//Bias select, 1/9 bias
	lcd_spi_write(0xA2, LCD_CMD);

	//COM direction normal
	lcd_spi_write(0xC0, LCD_CMD);

	//power control, VB ON, VR ON, VF ON
	lcd_spi_write(0x2F, LCD_CMD);

	//Regulation Ratio Rb/Ra
	lcd_spi_write(0x24, LCD_CMD);

	//set V0 output
	lcd_spi_write(0x81, LCD_CMD);
	lcd_spi_write(48,   LCD_CMD);

	//Drive mode
	lcd_spi_write(0xD2, LCD_CMD);
	lcd_spi_write(0x01, LCD_CMD);

	//start line
	lcd_spi_write(0x40, LCD_CMD);

	//page address, page 0
	lcd_spi_write(0xB0, LCD_CMD);

	//column address 4 higher bits = 0;
	lcd_spi_write(0x10, LCD_CMD);

	//column address 4 lower bits = 1
	lcd_spi_write(0x01, LCD_CMD);


	//lcd_screen_clear(0x67);

	//display ON
	lcd_spi_write(0xAF, LCD_CMD);
}
#else
void lcd_screen_init(void)
{
	lcd_spi_write(0xA0, LCD_CMD);
	lcd_spi_write(0xAE, LCD_CMD);
	lcd_spi_write(0xC8, LCD_CMD);
	lcd_spi_write(0xA2, LCD_CMD);
	lcd_spi_write(0x2F, LCD_CMD);
	lcd_spi_write(0x26, LCD_CMD);

	lcd_spi_write(0x81, LCD_CMD);
	lcd_spi_write(20, LCD_CMD);

#if 1
	lcd_spi_write(0xA6, LCD_CMD);
	lcd_spi_write(0xA5, LCD_CMD);//All pixel ON
	//lcd_spi_write(0xA4, LCD_CMD);
	lcd_spi_write(0x40, LCD_CMD);
	lcd_spi_write(0xB0, LCD_CMD);
	lcd_spi_write(0x10, LCD_CMD);
	lcd_spi_write(0x01, LCD_CMD);
	//lcd_screen_clear(0x0);
#endif
	lcd_spi_write(0xAF, LCD_CMD);
}

#endif
void lcd_init(void)
{
	lcd_pin_init();
	lcd_spi_init();
	lcd_set_rst(1);
	lcd_set_cs(1);
	lcd_set_a0(0);
	//lcd_reset();
	lcd_screen_init();
}

unsigned char lcd_disp[] = {0x01,0x02,0x03,0x04,0x05,0x77,0x88};
void lcd_screen_putc(int row, int col, uint8_t ch)
{
	int i,idx = 2;

	lcd_spi_write(0xB0 | lcd_pages[row & 0x7], LCD_CMD);
	lcd_spi_write(((col + 1) >> 4) | 0x10, LCD_CMD); //set column addr high byte
	lcd_spi_write((col + 1) & 0x0F, LCD_CMD);//set low byte

	for(i = 0;i < LCD_COL_DATA_SIZE;i++)
		lcd_spi_write(lcd_disp[i],LCD_DAT);
		//lcd_spi_write(lcd_asclib[idx][i],LCD_DAT);

}

void lcd_screen_puts(int row, int col, const char *s)
{

}

void lcd_test(void)
{
	rprintf("%s, start\r\n",__func__);
	lcd_init();

	while(1) {
		lcd_screen_putc(0,0,'A');
		mdelay(100);

		//lcd_screen_putc(0,0,'B');
		//mdelay(100);
		while(1);
	}
}

