
volatile static int gcnt = 0;

int main(void)
{
	while(gcnt != 0x10000000)
		gcnt++;

	return 0;
}
