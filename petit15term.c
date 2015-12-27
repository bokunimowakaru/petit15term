/***********************************************************************
Petit Ichigo Term for Raspberry Pi

by 国野亘
                   http://www.geocities.jp/bokunimowakaru/diy/ichigojam/

拡張ポートのシリアル端子およびFTDI製USBシリアル変換アダプタを使用して
動作確認しました。

TinyTermからの変更点
・終了処理の追加「---」でターミナル終了
・起動時の標準入力設定を保存し終了前に復帰する機能の追加
・起動及び終了時刻の表示機能の追加
・USB接続のIchigoJamへの対応
・IchigoJam用の設定変更（ボーレート・エコー・改行コード）

************************************************************************
下記のソースコードを利用しました。

// TinyTerm
// serial programming example for Raspberry Pi

// http://www.soramimi.jp/raspberrypi/rs232c/

************************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/time.h>
#include <time.h>

#define UART_SPEED 115200
#define UART_USB_MAX 10			// URT_USBの最大数(最大値9)

void log_date(char *s){
    time_t error_time;
    struct tm *error_time_st;
    
    time(&error_time);
    error_time_st = localtime(&error_time);
    printf("%4d/%02d/%02d %02d:%02d:%02d %s\n",
        error_time_st->tm_year+1900,
        error_time_st->tm_mon+1,
        error_time_st->tm_mday,
        error_time_st->tm_hour,
        error_time_st->tm_min,
        error_time_st->tm_sec,
        s
    );
}


int uart_open(char const *device, int bps, struct termios *saveattr)
{
	int fd;
	int speed;
	struct termios attr;

	fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd < 0) {
		return -1;
	}

	tcgetattr(fd, &attr);
	*saveattr = attr;

	speed = B115200;
	switch (bps) {
//	case      50:	speed =      B50;		break;
//	case      75:	speed =      B75;		break;
//	case     110:	speed =     B110;		break;
//	case     134:	speed =     B134;		break;
//	case     150:	speed =     B150;		break;
// 	case     200:	speed =     B200;		break;
	case     300:	speed =     B300;		break;
	case     600:	speed =     B600;		break;
	case    1200:	speed =    B1200;		break;
//	case    1800:	speed =    B1800;		break;
	case    2400:	speed =    B2400;		break;
	case    4800:	speed =    B4800;		break;
	case    9600:	speed =    B9600;		break;
	case   19200:	speed =   B19200;		break;
	case   38400:	speed =   B38400;		break;
	case   57600:	speed =   B57600;		break;
	case  115200:	speed =  B115200;		break;
	case  230400:	speed =  B230400;		break;
//	case  460800:	speed =  B460800;		break;
//	case  500000:	speed =  B500000;		break;
//	case  576000:	speed =  B576000;		break;
//	case  921600:	speed =  B921600;		break;
//	case 1000000:	speed = B1000000;		break;
//	case 1152000:	speed = B1152000;		break;
//	case 1500000:	speed = B1500000;		break;
//	case 2000000:	speed = B2000000;		break;
//	case 2500000:	speed = B2500000;		break;
//	case 3000000:	speed = B3000000;		break;
//	case 3500000:	speed = B3500000;		break;
//	case 4000000:	speed = B4000000;		break;
	}

	cfsetispeed(&attr, speed);
	cfsetospeed(&attr, speed);
	cfmakeraw(&attr);

	attr.c_cflag |= CS8 | CLOCAL | CREAD;
	attr.c_iflag = 0;
	attr.c_oflag = 0;
	attr.c_lflag = 0;
	attr.c_cc[VMIN] = 1;
	attr.c_cc[VTIME] = 0;

	tcsetattr(fd, TCSANOW, &attr);

	return fd;
}

int uart_close(int fd, struct termios *saveattr)
{
	tcsetattr(fd, TCSANOW, saveattr);
	return close(fd);
}

int main()
{
	fd_set readfds;
	int fd,loop=1,i;
	char c;
	struct termios stdinattr,stdin_bk;
	struct termios uartattr;
	//                    0123456789ABC
	char UART_DEVICE[15]="/dev/ttyUSB10";

	printf("Petit Ichigo Term for Raspberry Pi\n");
	for(i=UART_USB_MAX;i>=-1;i--){
		if(i>=0) snprintf(&UART_DEVICE[8],5,"USB%1d",i);
		else snprintf(&UART_DEVICE[8],5,"AMA0");
	//	log_date(UART_DEVICE);
		fd = uart_open(UART_DEVICE, UART_SPEED, &uartattr);
		if(fd >=0 ) break;
	}
	if (fd < 0){
		log_date("ERROR @ UART OPEN");
		return 1;
	}else{
		log_date(UART_DEVICE);
	}

	// set to non canonical mode for stdin
	tcgetattr(0, &stdinattr);
	tcgetattr(0, &stdin_bk);
	stdinattr.c_lflag &= ~ICANON;
//	stdinattr.c_lflag &= ~ECHO;
//	stdinattr.c_lflag &= ~ISIG;
	stdinattr.c_cc[VMIN] = 0;
	stdinattr.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &stdinattr);
	log_date("CONNECTED");
	log_date("Type '---' to exit.");

	c=0x10;write(fd, &c, 1);	// clear
	while (loop) {
		FD_ZERO(&readfds);
		FD_SET(0, &readfds); // stdin
		FD_SET(fd, &readfds);
		if (select(fd + 1, &readfds, 0, 0, 0) != 0) {
			if (FD_ISSET(0, &readfds)) {
				if (read(0, &c, 1) == 1) { // input from stdin
//					write(1, &c, 1);		// echo back
					if (c == '\n') {
						write(fd, "\n", 1); // no need to do
					} else {
						write(fd, &c, 1);
					}
					if(c=='-'){
						loop++;
						if(loop>3){
							loop=0;
							printf("\n");
						}
					}else loop=1;
				}
			}
			if (FD_ISSET(fd, &readfds)) {
				if (read(fd, &c, 1) == 1) {
					// output to stdout
					if (c == '\n') {		 // no need to do
						write(1, "\n", 1);
					} else {
						write(1, &c, 1);
					}
				}
			}
		}
	}

	// back to canonical mode for stdin
	tcsetattr(0, TCSANOW, &stdin_bk);
	
	if(uart_close(fd, &uartattr)){
		log_date("ERROR @ UART CLOSE");
		return 1;
	}
	log_date("CLOSED");
	return(0);
}


