#include <stm32f10x.h>
#include <common.h>

const char *logo = "0x12345678abcd";
int a = 1;
int b = 2;
int c = 3;
static int d = 5;
int array[10];

int main(int argc, const char *argv[])
{

	c = a + b;
	d = d + c;
	array[0] = d;

	return 0;
}
