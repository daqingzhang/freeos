#include <common.h>

void dump_mem(unsigned int *addr,int size)
{
	rprintf("%s\r\n",__func__);

	while(size--) {
		rprintf("%x = %x\r\n",(unsigned int )(addr),*addr);
		addr++;
	}
}
