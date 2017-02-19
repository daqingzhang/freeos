#include <lcd.h>

//width=5,height=8
unsigned char lcd_displib[46][5] ={
{0x3e,0x41,0x41,0x3e,0x00},//0
{0x00,0x42,0x7f,0x40,0x00},//1
{0x66,0x51,0x49,0x46,0x00},//2
{0x49,0x49,0x49,0x36,0x00},//3
{0x38,0x24,0x7f,0x20,0x00},//4
{0x27,0x49,0x49,0x49,0x30},//5
{0x3e,0x49,0x49,0x49,0x30},//6
{0x61,0x11,0x09,0x07,0x00},//7
{0x36,0x49,0x49,0x49,0x36},//8
{0x26,0x49,0x49,0x49,0x3e},//9
{0x78,0x16,0x11,0x16,0x78},//A
{0x7f,0x49,0x49,0x36,0x00},//B
{0x3e,0x41,0x41,0x41,0x22},//C
{0x7f,0x41,0x41,0x22,0x1c},//D
{0x7f,0x49,0x49,0x49,0x49},//E
{0x7f,0x09,0x09,0x01,0x00},//F
{0x3e,0x41,0x51,0x51,0x32},//G
{0x7f,0x08,0x08,0x7f,0x00},//H
{0x00,0x41,0x7f,0x41,0x00},//I
{0x20,0x41,0x3f,0x01,0x00},//J
{0x7f,0x08,0x14,0x22,0x41},//K
{0x7f,0x40,0x40,0x40,0x00},//L
{0x7f,0x01,0x0e,0x01,0x7f},//M
{0x7f,0x02,0x1c,0x20,0x7f},//N
{0x3e,0x41,0x41,0x41,0x3e},//O
{0x7f,0x09,0x09,0x09,0x06},//P
{0x3e,0x41,0x51,0x22,0x7c},//Q
{0x7f,0x09,0x19,0x29,0x46},//R
{0x26,0x49,0x49,0x49,0x32},//S
{0x01,0x01,0x7f,0x01,0x01},//T
{0x3f,0x40,0x40,0x40,0x3f},//U
{0x0f,0x30,0x40,0x30,0x0f},//V
{0x7f,0x40,0x38,0x40,0x7f},//W
{0x63,0x14,0x08,0x14,0x63},//X
{0x03,0x0c,0x70,0x0c,0x03},//Y
{0x61,0x51,0x49,0x45,0x43},//Z
{0x00,0x00,0x00,0x00,0x00},//space
{0x04,0x04,0x1f,0x04,0x04},//+
{0x04,0x04,0x04,0x04,0x04},//-
{0x00,0x00,0x2f,0x00,0x00},//!
{0x00,0x00,0x20,0x00,0x00},//.
{0x12,0x0c,0x3f,0x0c,0x12},//*
{0x10,0x08,0x04,0x02,0x01},// /
{0x00,0x00,0x0a,0x00,0x00},// :
{0x10,0x09,0x04,0x12,0x01},// %
{0x14,0x14,0x14,0x14,0x14},//=
};