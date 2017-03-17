#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>


int main(void)
{
	char portname[32] = "/dev/ttyS0";

	int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
	assert(fd > -1);
	
	struct termios serial;
	memset(&serial, 0, sizeof(struct termios));

	//115200Baud, 8Bit
	serial.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
	//ignore parity, map CR to NL
	serial.c_iflag = IGNPAR | ICRNL;
	//unuse output option
	serial.c_oflag = 0;
	//raw input (unuse signal bits)
	serial.c_lflag = ~(ICANON | ECHO | ECHOE | ISIG);

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &serial);

	
	char buf[32];
	int recvSize = 18;

	int n, i;

	
	int cnt = 0;

	for (;;) {
		read(fd, buf, recvSize);
		printf("%d ", cnt++);

		for( i = 0; i<recvSize; i++) {
			printf("%d ", buf[i]);
		}
		printf("magic : %c%c%c\n", buf[0], buf[1], buf[2]); 
		if (buf[3] == 0)
			puts("AorM : manual");
		else
			puts("AorM : auto");

		if (buf[4] == 0)
			puts("E-STOP : off");
		else
			puts("E-STOP : on");

		if (buf[5] == 0)
			puts("E-STOP : forward drive");
		else if (buf[5] == 1)
			puts("E-STOP : neutral");
		else
			puts("E-STOP : backward drive");

		printf("SPEED : %X %X\n", buf[6], buf[7]);
		printf("STEER : %hd\n", (buf[8]<<8&0xFF00)|buf[9]);
		printf("BRAKE : %d\n", buf[10]);
		printf("STEER : %X %X %X %X\n", buf[11], buf[12], buf[13], buf[14]);

		printf("ALIVE : %hhu\n", buf[15]);
		printf("ETX0 : %X\n", buf[16]);
		printf("ETX1 : %X\n", buf[17]);


		putchar('\n');
	}


	close(fd);

	return 0;

}
