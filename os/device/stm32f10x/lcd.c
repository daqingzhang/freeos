#include <common.h>
#include <string.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>
#include <lcd.h>

#define LCD_SPI	SPI1
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

#define LCD_CMD_END	0xEE
#define LCD_CMD_RESET	0xE2
#define LCD_CMD_APON	0xA5
#define LCD_CMD_APOFF	0xA4
#define LCD_CMD_BIAS	0xA2

#define LCD_CMD_PAGE	0xB0
#define LCD_CMD_SLINE	0x40
#define LCD_CMD_COL_ADDRH	0x10
#define LCD_CMD_COL_ADDRL	0x00
#define LCD_CMD_NOP		0xE3

enum LCD_CMD_TYPE
{
	LCD_DAT = 0,
	LCD_CMD,
};

static uint8_t lcd_pages[] = {0,1,2,3,4,5,6,7};

static void lcd_pin_init(void)
{
	GPIO_InitTypeDef Init;

	//SPI in master mode
	// enable peripherial clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

	// RST
	Init.GPIO_Pin	= LCD_PIN_RST;
	Init.GPIO_Mode	= GPIO_Mode_Out_PP;
	Init.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(LCD_GPIO1,&Init);
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

static void lcd_spi_init(void)
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

static void lcd_set_cs(int level)
{
	if(level)
		GPIO_WriteBit(LCD_GPIO2,LCD_PIN_CS,Bit_SET);//cs = 1
	else
		GPIO_WriteBit(LCD_GPIO2,LCD_PIN_CS,Bit_RESET);//cs = 0
}

static void lcd_set_a0(int level)
{
	if(level)
		GPIO_WriteBit(LCD_GPIO1,LCD_PIN_A0,Bit_SET);//A0 = 1
	else
		GPIO_WriteBit(LCD_GPIO1,LCD_PIN_A0,Bit_RESET);//A0 = 0
}

static void lcd_set_rst(int level)
{
	if(level)
		GPIO_WriteBit(LCD_GPIO1,LCD_PIN_RST,Bit_SET);
	else
		GPIO_WriteBit(LCD_GPIO1,LCD_PIN_RST,Bit_RESET);
}

static void lcd_spi_write(uint8_t dat, enum LCD_CMD_TYPE cmd)
{
	uint8_t dummy;

	//wait until txfifo is emtpy
	while((LCD_SPI->SR & SPI_I2S_FLAG_TXE) == 0);

	// set A0 line
	if(cmd == LCD_CMD)
		lcd_set_a0(0);
	else
		lcd_set_a0(1);
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

static void lcd_set_page_addr(int page)
{
	lcd_spi_write(LCD_CMD_PAGE | page,  LCD_CMD);
}

static void lcd_set_col_addr(int column)
{
	int h = column / 0x0F;
	int l = column % 0x0F;

	lcd_spi_write(LCD_CMD_COL_ADDRH | h, LCD_CMD);
	lcd_spi_write(LCD_CMD_COL_ADDRL | l, LCD_CMD);
}

static void lcd_set_row_data(uint8_t data)
{
	lcd_spi_write(data, LCD_DAT);
}

static int lcd_get_disp_data(unsigned char c)
{
	int idx = -1;

	if((c >= '0') && (c <='9')) {
		idx = c - 0x30;
	} else if((c >= 'A') && (c <= 'Z')) {
		idx = c - 0x37;
	} else if((c >= 'a') && (c <= 'z')) {
		idx = c - 0x57;
	} else {
		switch(c) {
		case ' ':
			idx = 36;
			break;
		case '+':
			idx = 37;
			break;
		case '-':
			idx = 38;
			break;
		case '!':
			idx = 39;
			break;
		case '.':
			idx = 40;
			break;
		case '*':
			idx = 41;
			break;
		case '/':
			idx = 42;
			break;
		case ':':
			idx = 43;
			break;
		case '%':
			idx = 44;
			break;
		case '=':
			idx = 45;
			break;
		case '?':
		case ';':
		case '@':
		case '#':
		case '$':
		case '^':
		case '&':
		case '(':
		case ')':
		default:
			idx = -1;
			break;
		}
	}
	return idx;
}

static int page_invalid(int page)
{
	if((page < 0) || (page > LCD_PAGE_NR - 1))
		return -1;
	return 0;
}

static int column_invalid(int column)
{
	//if((column < 0) || (column > LCD_COL_END))
	if((column < 0) || (column > LCD_COL_END - LCD_DATA_HEIGHT))
		return -1;
	return 0;
}

void lcd_clear_screen(unsigned char fill)
{
	int i,j;

	for(i = 0; i < LCD_PAGE_NR;i++) {
		lcd_set_page_addr(lcd_pages[i]);
		lcd_set_col_addr(0);
		for(j = 0;j < LCD_X_DOT;j++)
			lcd_set_row_data(fill);
	}
}

void lcd_clear_page(int page,unsigned char fill)
{
	int j;

	lcd_set_page_addr(page);
	lcd_set_col_addr(0);
		for(j = 0;j < LCD_X_DOT;j++)
			lcd_set_row_data(fill);
}

void lcd_reset(void)
{
	mdelay(10);
	lcd_set_rst(0);
	mdelay(200);
	lcd_set_rst(1);
	mdelay(50);
}

void lcd_set_panel(void)
{
	lcd_reset();

	lcd_spi_write(0xAE, LCD_CMD); //display OFF
	lcd_spi_write(0xA2, LCD_CMD); //bias, 1/9
	lcd_spi_write(0xA0, LCD_CMD); //ADC select
	//lcd_spi_write(0xC0, LCD_CMD); //com1->com64
	lcd_spi_write(0xC8, LCD_CMD); //com1->com64
	lcd_spi_write(0x24, LCD_CMD); //Rb/Ra
	lcd_spi_write(0x81, LCD_CMD); //contrast
	lcd_spi_write(50,   LCD_CMD);
	lcd_spi_write(0x2F, LCD_CMD); //Voltage
	lcd_spi_write(0xA6, LCD_CMD); //Normal display
	//lcd_spi_write(0xA5, LCD_CMD); //All pixel ON
	lcd_spi_write(0xA4, LCD_CMD); //All pixel off
	lcd_spi_write(0x40, LCD_CMD); //Display start line = 0
	lcd_spi_write(0xB0, LCD_CMD); //Page address = 0
	lcd_spi_write(0x10, LCD_CMD); //Column address high
	lcd_spi_write(0x01, LCD_CMD); //column address low
	lcd_spi_write(0xAF, LCD_CMD); //Display ON

	lcd_clear_screen(0x0);
}

void lcd_init(void)
{
	lcd_pin_init();
	lcd_spi_init();
	lcd_set_cs(1);
	lcd_set_a0(0);
	lcd_set_panel();
}

extern unsigned char lcd_displib[][LCD_DATA_WIDTH];

int lcd_putc(int page, int col, const char ch)
{
	int i,idx,p;

	p = lcd_pages[page];
	if(page_invalid(p))
		return -LCD_ERR_INVALID_PAGE;
	lcd_set_page_addr(p);

	if(column_invalid(col))
		return -LCD_ERR_INVALID_COL;
	lcd_set_col_addr(col);

	idx = lcd_get_disp_data(ch);
	if(idx < 0)
		return -LCD_ERR_INVALID_DATA;

	for(i = 0;i < LCD_DATA_WIDTH;i++)
		lcd_set_row_data(lcd_displib[idx][i]);
	return 0;
}

int lcd_puts(int page, int col, const char *s)
{
	int len,retval;

	if((!s) || (!(len = (int)strlen(s))))
		return -LCD_ERR_INVALID_DATA;

	while(len--) {
		if(column_invalid(col)) {
			if(page_invalid(page))
				break;
			col = LCD_COL_START;
			page++;
		}
		retval = lcd_putc(page, col, *(s++));
		if(retval < 0)
			return retval;

		col = LCD_INC_COL(col);
	}
	return 0;
}

void lcd_test(void)
{
	int page,col;

	rprintf("%s, start\r\n",__func__);
	lcd_init();
#if 0
	while(1) {
		for(page = 0;page < 8;page++)
			for(col = 0;col < (128 - 8);col += 8) {
				lcd_clear_screen(0);
				mdelay(10);
				lcd_putc(lcd_pages[page],col,'A');
				mdelay(500);
			}
	}
#endif
#if 1
	//lcd_putc(0,0,'0');
	//lcd_putc(0,0,'0');
	lcd_puts(0,0,"1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ+-*/:.!%=");
	//lcd_clear_page(1,0xFF);
	lcd_puts(4,0,"helloworld");
	lcd_puts(5,0,"helle c program");
#endif

	for(;;);
}
