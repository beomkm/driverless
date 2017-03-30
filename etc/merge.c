#include <stdio.h>

int main()
{
	char high = 10;
	char low = 20;
	printf("%d", (high<<8)|low);
	return 0;
}
