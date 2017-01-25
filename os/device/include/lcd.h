#ifndef __LCD_H__
#define __LCD_H__

#define LCD_X_DOT_CNT 128
#define LCD_Y_DOT_CNT 64

#define LCD_X_DATA_SIZE	8
#define LCD_Y_DATA_SIZE	8

#define LCD_ROW_DATA_NR (LCD_Y_DOT_CNT / LCD_Y_DATA_SIZE)
#define LCD_COL_DATA_NR (LCD_X_DOT_CNT / LCD_X_DATA_SIZE)

#define LCD_ROW_DATA_START 0
#define LCD_ROW_DATA_SIZE  LCD_Y_DATA_SIZE
#define LCD_ROW_DATA_END   LCD_Y_DOT_CNT

#define LCD_COL_DATA_START 0
#define LCD_COL_DATA_SIZE  LCD_X_DATA_SIZE
#define LCD_COL_DATA_END   LCD_X_DOT_CNT

#define LCD_ASCLIB_NR 37
#define LCD_ASCLIB_SIZE 32
extern unsigned char lcd_asclib[LCD_ASCLIB_NR][LCD_ASCLIB_SIZE];

void lcd_test(void);

#endif
