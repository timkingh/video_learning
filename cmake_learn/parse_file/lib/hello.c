#include <stdio.h>

void print_hello()
{
	printf("%s line %d Hello!\n", __FUNCTION__, __LINE__);
}