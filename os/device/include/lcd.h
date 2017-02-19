#ifndef __LCD_H__
#define __LCD_H__

#define LCD_X_DOT	128
#define LCD_Y_DOT	64
#define LCD_PAGE_NR	8

#define LCD_DATA_X_DOT	5
#define LCD_DATA_Y_DOT	8

//5x8
#define LCD_DATA_WIDTH  LCD_DATA_X_DOT
#define LCD_DATA_HEIGHT LCD_DATA_Y_DOT

#define LCD_COL_START 0
#define LCD_COL_END   (LCD_X_DOT - LCD_DATA_X_DOT)

#define LCD_INC_COL(col) ((col) + LCD_DATA_WIDTH + 1)

#define LCD_ERR_INVALID_PAGE	1
#define LCD_ERR_INVALID_COL	2
#define LCD_ERR_INVALID_DATA	3

void lcd_init(void);
void lcd_clear_screen(unsigned char fill);
void lcd_clear_page(int page,unsigned char fill);
int  lcd_putc(int page, int col, const char ch);
int  lcd_puts(int page, int col, const char *s);

void lcd_test(void);

#endif
