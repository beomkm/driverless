#include "LidarManager.h"
#include<time.h>

int main(void)
{
	LidarManager l;
	l.setUpLidar();
	printf("set up complete!\n");
	l.start();
	printf("start!\n");

	while(1){

		l.run();

	}


	l.stop();

}
