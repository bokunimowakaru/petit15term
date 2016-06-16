/**************************************************************************************
petitPanCake for Raspberry Pi

                                                       Copyright (c) 2015 Wataru KUNINO
***************************************************************************************/
#include <stdio.h>                                  // 標準入出力用
#include <fcntl.h>                                  // シリアル通信用(Fd制御)
#include <termios.h>                                // シリアル通信用(端末IF)
#include <unistd.h>                                 // read,usleep用
#include <string.h>                                 // strncmp,bzero用
#include <sys/time.h>                               // fd_set,select用
#include <ctype.h>                                  // isprint,isdigit用
static int ComFd;                                   // シリアル用ファイルディスクリプタ
static struct termios ComTio_Bk;                    // 現シリアル端末設定保持用の構造体
#define TX_WAIT 0
#define TXL_WAIT 0


int open_serial_port(){
    struct termios ComTio;                          // シリアル端末設定用の構造体変数
    speed_t speed = B115200;                        // 通信速度の設定
    char modem_dev[15]="/dev/ttyUSBx";              // シリアルポートの初期値
    int i;
    for(i=12;i>=-1;i--){
        if(i>=10) snprintf(&modem_dev[5],8,"rfcomm%1d",i-10);   // ポート探索(rfcomm0-2)
        else if(i>=0) snprintf(&modem_dev[5],8,"ttyUSB%1d",i);  // ポート探索(USB0～9)
        else snprintf(&modem_dev[5],8,"ttyAMA0");   // 拡張IOのUART端子に設定
        ComFd=open(modem_dev, O_RDWR|O_NONBLOCK);   // シリアルポートのオープン
        if(ComFd >= 0){                             // オープン成功時
            printf("com=%s\n",modem_dev);           // 成功したシリアルポートを表示
            tcgetattr(ComFd, &ComTio_Bk);           // 現在のシリアル端末設定状態を保存
            ComTio.c_iflag = 0;                     // シリアル入力設定の初期化
            ComTio.c_oflag = 0;                     // シリアル出力設定の初期化
            ComTio.c_cflag = CLOCAL|CREAD|CS8;      // シリアル制御設定の初期化
            ComTio.c_lflag = 0;                     // シリアルローカル設定の初期化
            bzero(ComTio.c_cc,sizeof(ComTio.c_cc)); // シリアル特殊文字設定の初期化
            cfsetispeed(&ComTio, speed);            // シリアル入力の通信速度の設定
            cfsetospeed(&ComTio, speed);            // シリアル出力の通信速度の設定
            ComTio.c_cc[VMIN] = 0;                  // リード待ち容量0バイト(待たない)
            ComTio.c_cc[VTIME] = 0;                 // リード待ち時間0.0秒(待たない)
            tcflush(ComFd,TCIFLUSH);                // バッファのクリア
            tcsetattr(ComFd, TCSANOW, &ComTio);     // シリアル端末に設定
            break;                                  // forループを抜ける
        }
    }
    return ComFd;
}

char read_serial_port(void){
    char c='\0';                                    // シリアル受信した文字の代入用
    fd_set ComReadFds;                              // select命令用構造体ComReadFdを定義
    struct timeval tv;                              // タイムアウト値の保持用
    FD_ZERO(&ComReadFds);                           // ComReadFdの初期化
    FD_SET(ComFd, &ComReadFds);                     // ファイルディスクリプタを設定
    tv.tv_sec=0; tv.tv_usec=2500;                   // 受信のタイムアウト設定(1.8ms以上)
    if(select(ComFd+1, &ComReadFds, 0, 0, &tv)) read(ComFd, &c, 1); // データを受信
    // usleep(5000);                                // 5msの(IchigoJam処理)待ち時間
    return c;                                       // 戻り値＝受信データ(文字変数c)
}

int close_serial_port(void){
    tcsetattr(ComFd, TCSANOW, &ComTio_Bk);
    return close(ComFd);
}

int ahex2i(char c){
	if(c>='0' && c<='9') return c-'0';
	if(c>='a' && c<='f') return c-'a'+10;
	if(c>='A' && c<='F') return c-'F'+10;
	return -1;
}

int main(int argc,char **argv){
    char s[256];                                     // 文字データ用
    char PC_LINE[]={0x80,0x08,0x01};
    int i,j,k,loop;
    char c;                                         // 文字入力用の文字変数
    FILE *fp;
    
    printf("petitPanCake for Raspberry Pi\n");
    if( argc < 2){
		fprintf(stderr,"Usage : %s input_files",argv[0]);
		return -1;
	}
    if(open_serial_port() <= 0){
        fprintf(stderr,"UART OPEN ERROR\n");
        return -1;
    }
    // write(ComFd, "\r\n", 2);
    for(loop=0;loop < (argc-1);loop++){
		fp=fopen(argv[loop+1],"r");
    	if(fp==0){
	        fprintf(stderr,"FILE OPEN ERROR\n");
	        return -1;
		}
		
		while(feof(fp)==0){
			fgets(s,256,fp);
			printf("%s",s);
			if(strncmp(s,"?\"",2)==0) j=2;
			else if(strncmp(s,"? \"",3)==0) j=3;
			else j=0;
			if(strncmp(&s[j],"PC LINE",7)==0){
				printf("(BIN)");
				write(ComFd, PC_LINE, 3);
				for(i=j+8;i<255;i+=3){
					k=ahex2i(s[i]); if( k<0 ) break;
					c=(char)ahex2i(s[i+1]); if( c<0 ) break;
					c+=(char)(k<<4);
					write(ComFd, &c, 1);
				//	usleep(TX_WAIT);
				}
				j=0;
			}else for(i=j;i<255;i++){
				if(s[i]=='\"' || s[i]=='\0' ) break;
				write(ComFd, &s[i], 1);
				// write(0, &s[i], 1);
			//	usleep(TX_WAIT);
			}
			write(ComFd, "\r\n", 2);
		//	usleep(TXL_WAIT);
			c=read_serial_port();
	        while(c){
                if( isprint(c) ) printf("%c",c);    // 表示可能な文字の時に表示する
                if( c=='\n' ) printf("\n");    // 改行時に改行と5文字インデントする
                c=read_serial_port();               // シリアルからデータを受信
	        }
	    }
	    fclose(fp);
    }
    printf("DONE\n");
    close_serial_port();
    return 0;
}
