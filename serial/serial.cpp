#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>


int set_interface_attribs(int fd, int speed, int parity)
{
	struct termios tty;
	memset(&tty, 0, sizeof tty);
	if (tcgetattr(fd, &tty) != 0) {
		//error_message ("error %d from tcgetattr", errno);
		return -1;
	}

	cfsetospeed(&tty, speed);
	cfsetispeed(&tty, speed);

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
	// disable IGNBRK for mismatched speed tests; otherwise receive break
	// as \000 chars
	tty.c_iflag &= ~IGNBRK;         // disable break processing
	tty.c_lflag = 0;                // no signaling chars, no echo,
						// no canonical processing
	tty.c_oflag = 0;                // no remapping, no delays
	tty.c_cc[VMIN]  = 0;            // read doesn't block
	tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

	tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
							// enable reading
	tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
	tty.c_cflag |= parity;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;

	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		//error_message ("error %d from tcsetattr", errno);
		return -1;
	}
	return 0;
}

void set_blocking(int fd, int should_block)
{
	struct termios tty;
	memset(&tty, 0, sizeof tty);
	if (tcgetattr(fd, &tty) != 0)
	{
	//error_message ("error %d from tggetattr", errno);
	return;
	}

	tty.c_cc[VMIN]  = should_block ? 1 : 0;
	tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

	//if (tcsetattr (fd, TCSANOW, &tty) != 0)
	//error_message ("error %d setting term attributes", errno);
}


int main()
{
	char *portname = "/dev/ttyS0";

	int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		//err msg
		fprintf(stderr, "fd err\n");
		return 0;
	}

	set_interface_attribs(fd, B115200, 0);
	set_blocking(fd, 0);

	//write(fd, "hello!\n", 7);
	//usleep((7+25)*100); //if you send 7, should sleep

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


			
	return 0;

}
