#include <stdarg.h>
#include <string.h>
#include <serial.h>
#include <stm32f10x_system.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>

/*
 * USART Basic Driver
 **************************************************************
 */
static int usart_config_nvic(int id)
{
	switch(id) {
	case USART1_ID: //USART1
		system_nvic_set_channel(USART1_IRQn,NVIC_CHANNEL_PRIO_USART1,0,1);
		break;
	case USART2_ID: //USART2
		system_nvic_set_channel(USART2_IRQn,NVIC_CHANNEL_PRIO_USART2,0,1);
		break;
	default:
		return -1;
	}
	return 0;
}

static int usart_config_pins(int id)
{
	GPIO_InitTypeDef init;

	switch(id) {
	case USART1_ID:
		/* enable gpio apb clock
		 * USART1 Rx (PA.10) input-floating
		 * USART1 Tx (PA.09) alt-push-pull
		 */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

		init.GPIO_Pin    = GPIO_Pin_9;
		init.GPIO_Mode   = GPIO_Mode_AF_PP;
		init.GPIO_Speed  = GPIO_Speed_50MHz;

		GPIO_Init(GPIOA, &init);
		init.GPIO_Pin    = GPIO_Pin_10;
		init.GPIO_Mode   = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &init);
		break;
	case USART2_ID:
		/* enable gpio apb clock
		 * USART2 Tx (PA.02) as alt-push-pull
		 * USART2 Rx (PA.03) as input-floating
		 */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

		init.GPIO_Pin    = GPIO_Pin_2;
		init.GPIO_Mode   = GPIO_Mode_AF_PP;
		init.GPIO_Speed  = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &init);

		init.GPIO_Pin    = GPIO_Pin_3;
		init.GPIO_Mode   = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &init);
		break;
	default:
		return -1;
	}
	return 0;
}

static int usart_config_format(int id,int baudrate)
{
	USART_InitTypeDef init;

	// config: 8,N,1,no CTS/RTS
	init.USART_BaudRate    = (uint16_t)baudrate;
	init.USART_WordLength  = USART_WordLength_8b;
	init.USART_Parity      = USART_Parity_No;
	init.USART_StopBits    = USART_StopBits_1;
	init.USART_Mode        = USART_Mode_Rx | USART_Mode_Tx;
	init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	switch(id) {
	case USART1_ID:
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		USART_Init(USART1, &init);
		USART_Cmd(USART1, ENABLE);
		break;
	case USART2_ID:
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		USART_Init(USART2, &init);
		USART_Cmd(USART2, ENABLE);
		break;
	default:
		return -1;
	}
	return 0;
}

static int usart_reset(int id)
{
	switch(id) {
	case USART1_ID:
		USART_DeInit(USART1);
		break;
	case USART2_ID:
		USART_DeInit(USART2);
		break;
	default:
		return -1;
	}
	return 0;
}

static int usart_init(int id,int baudrate)
{
	int r;

	r = usart_reset(id);
	if(r)
		return r;
	r = usart_config_nvic(id);
	if(r)
		return r;
	r = usart_config_pins(id);
	if(r)
		return r;
	r = usart_config_format(id,baudrate);
	if(r)
		return r;
	return 0;
}

static char __usart1_getc(void)
{
	// wait until rx fifo not empty
	while((USART1->SR & USART_FLAG_RXNE) == 0)
		;
	return (USART1->DR & 0xFF);
}

static void __usart1_putc(char ch)
{
	// wait until tx fifo is empty
	while(!(USART1->SR & USART_FLAG_TXE))
		;
	USART1->DR = ch;
}

static char __usart2_getc(void)
{
	// wait until rx fifo not empty
	while((USART2->SR & USART_FLAG_RXNE) == 0)
		;
	return (USART2->DR & 0xFF);
}

static void __usart2_putc(char ch)
{
	// wait until tx fifo is empty
	while(!(USART2->SR & USART_FLAG_TXE))
		;
	USART2->DR = ch;
}

/*
 * Serial Driver Implementation
 **************************************************************
 */

void serial_init(int id)
{
	if(id == USART1_ID)
		usart_init(USART1_ID,CONFIG_USART1_BAUDRATE);
	else if(id == USART2_ID)
		usart_init(USART2_ID,CONFIG_USART2_BAUDRATE);
}

char serial_getc(int id)
{
	char ch;

	if(id == USART1_ID)
		ch = __usart1_getc();
	else if(id == USART2_ID)
		ch = __usart2_getc();
	else
		ch = 0;

	return ch;
}

static void serial_putc_hw(int id,char ch)
{
	if(id == USART1_ID)
		__usart1_putc(ch);
	else if(id == USART2_ID)
		__usart2_putc(ch);
}

void serial_putc(int id,char ch)
{
	if(ch == '\n') {
		serial_putc_hw(id,'\r');
	}
	serial_putc_hw(id,ch);
}

int serial_tstc(int id)
{
	int r = 0;

	if(id == USART1_ID) {
		if(USART1->SR & USART_FLAG_RXNE)
			r = 1;
	} else if(id == USART2_ID) {
		if(USART2->SR & USART_FLAG_RXNE)
			r = 1;
	}
	return r;
}

void serial_puts(int id,const char *pstr)
{
	while(*pstr != '\0') {
		serial_putc(id,*pstr++);
	}
}

static void backspace(int id,int cnts)
{
	int i;

	for(i=0; i<cnts; i++) {
		serial_putc(id,'\b');
		serial_putc(id,' ');
		serial_putc(id,'\b');
	}
}

int serial_gets(int id,char *pstr)
{
	int length;

	length = 0;
	while(1) {
		pstr[length] = serial_getc(id);
		if(pstr[length] == '\r') {
			pstr[length] = 0x00;
			break;
		} else if( pstr[length] == '\b' ) {
			if(length>0) {
				length--;
				backspace(id,1);
			}
		} else {
			serial_putc(id,pstr[length]);
			length++;
		}
		if(length > 32)
			return -1;
	}
	return 0;
}

void oct2asc(uint8_t *pval,char *pstr,int len)
{
	char str[64];
	int i = 0;
	uint8_t ch = 0;
	uint64_t mask = 0,n,num = *(uint64_t *)pval;

	len = (len > 8) ? (8):len;
	while(i < len) {
		mask |= (0xFF << (i * 8));
		i++;
	}
	num &= mask;
	i = 0;
	while(num != 0) {
		n = num / 10;
		ch = num - n *10;
		num = n;
		ch += (ch < 10)?('0'):('A'-10);
		str[i] = ch;
		i++;
	}
	while(i > 0) {
		--i;
		*pstr++ = str[i];
	}
	*pstr = '\0';
}

void hex2asc(uint8_t *phex,char *pasc,int hexlen)
{
	uint8_t temp;
	char ch;
	*(pasc+hexlen) = '\0';
	phex += hexlen;
	while(hexlen--) {
		temp = *--phex;
		ch = (temp >> 4) & 0xF;
		ch += (ch < 10)?(0x30):(0x37);
		*pasc++ = ch;
		ch = temp & 0xF;
		ch += (ch < 10)?(0x30):(0x37);
		*pasc++ = ch;
	}
}

uint64_t asc2hex(uint8_t *pstr,int len)
{
	int i = 0;
	uint8_t *p,temp;
	uint64_t val = 0;

	p = pstr;
	while(*p != '\0'){
		i++;
		p++;
	}
	if(len > 16)
		len = 16;
	if(i > len)
		i = len;

	if((pstr[0] == '0')&&(pstr[1] == 'x'))
		p = pstr + 2;
	else if((pstr[0] == '0')&&(pstr[1] == 'X'))
		p = pstr + 2;
	else
		p = pstr;

	while(i--) {
		temp = *p++;
		if((temp >= '0')&&(temp <= '9'))
			temp = temp - 0x30;
		else if((temp >= 'A')&&(temp <= 'F'))
			temp = temp - 0x37;
		else if((temp > 'a')&&(temp <= 'f'))
			temp = temp - 0x20 - 0x37;
		else
			return val;
		val |= temp;
		val <<= 4;
	}
	return val;
}

static char *flex_i2a(unsigned num, char *pstr, int radix, int len, int digits)
{
	char ch;
	char *p;
	int sign = 0;

	if (radix < 0) {
		radix = -radix;
		sign = 1;
	}

	// limit the string and number of displayed digits
	if (digits > len-1) digits = len-1;
	p = pstr + len;
	*--p = 0;

	// print the number including sign
	while (pstr != p)
	{
		unsigned int n = num/radix;
		ch = num - n*radix;
		num = n;
		ch += (ch < 10)? '0': 'A'-10;
		*--p = ch;
		if (num == 0) break;
	}

	// output the sign
	if (sign && pstr != p) {
		*--p = '-';
	}

	if (digits == 0)
		return p;

	ch = (radix == 16)? '0': ' ';
	len = digits - (pstr+len-p-1); // target_digits - current_chars
	for (; len > 0; len--)
		*--p = ch;

	return p;
}

/*
 rprintf() is a tiny version of printf() for use in the boot_rom.
 The format-string is very limited subset of printf():%<size><format>
 where <size> is a single digit size specifier between 0 and 9. <size> is
 accepted for all <formats> but ignored for strings.

 The following <format> codes is supported:
    s    - string
    d    - decimal number, signed
    u    - decimal number, unsigned
    x    - hexadecimal number, unsigned
	lu	 - decimal number, unsigned long
	llu	 - decimal number, unsigned long long
	llx	 - hexadecimal number, unsigned long long
    %    - literal '%'
	c	 - character
 Examples:
        rprintf("The answer to life, universe and %s is %d!\n", "everything", 42);
        rprintf("%d is %u in unsigned (hex: 0x%8x)\n", -42, -42, -42);
    Output:
        "The answer to life, universe and everything is 42!"
        "-42 is 4294967254 in unsigned (hex: 0xFFFFFFD6)"

*/

void rprintf(const char *fmt, ...)
{
	va_list ap;
	char str[64];
	int iarg;
	int id = USART1_ID;

	va_start(ap, fmt);

	const char *s = fmt;
	while(*s) {
		if (*s == '%') {
			int size = 0;
			int radix = 10;
			const char *p = 0;

			++s;
			if (*s == '0') s++;
			if (*s >= '0' && *s <= '9') { size = *s++ - '0'; };

			switch(*s) {
			case 's':
				p = va_arg(ap, const char *);
				break;
			case 'x':
				radix = 16;
			case 'd':
			case 'u':
				iarg = va_arg(ap, int);
				if (*s == 'd' && iarg < 0) {
					radix = -10;
					iarg = -iarg;
				}
				p = flex_i2a(iarg, str, radix, sizeof(str), size);
				break;
			case '%':
				serial_putc(id,'%');
				break;
			case 'l':
				if((*(s+1) == 'l') && (*(s+2) == 'x')) {
					uint64_t num = 0;
					num = va_arg(ap,unsigned long long);
					hex2asc((uint8_t *)(&num), str,sizeof(num));
					p = str;
					s += 2;
				}
				else if((*(s+1) == 'l') && (*(s+2) == 'u')) {
					uint64_t num = 0;
					num = va_arg(ap,unsigned long long);
					oct2asc((uint8_t *)(&num), str,sizeof(num));
					p = str;
					s += 2;
				}
				else if(*(s+1) == 'u') {
					iarg = va_arg(ap,int);
					radix = 16;
					p = flex_i2a(iarg, str, radix, sizeof(str), size);
					s += 1;
				} else {
					str[0] = 'l';
					str[1] = 0;
					p = str;
				}
				break;
			case 'c':
				iarg = va_arg(ap, int);
				str[0] = iarg;
				str[1] = 0;
				p = str;
			default: ;
			}
			if (p)
				serial_puts(id,p);
		} else {
			serial_putc(id,*s);
		}
		++s;
	}
	va_end(ap);
}
