/***********************************************************************
Petit Ichigo Firmware Writer for Raspberry Pi

by 国野亘
                   http://www.geocities.jp/bokunimowakaru/diy/ichigojam/

拡張ポートのシリアル端子およびFTDI製USBシリアル変換アダプタを使用して
動作確認しました。

************************************************************************/

int TARGET_VERSION =111;              // ターゲットのバージョン 1.1.1
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>                                 // strncmp,bzero用
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/time.h>
#include <time.h>
#include "kbhit.c"
static int ComFd;                                   // シリアル用ファイルディスクリプタ
static struct termios ComTio_Bk;                    // 現シリアル端末設定保持用の構造体

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


int uart_open(char const *device, struct termios *saveattr)
{
    struct termios ComTio;                          // シリアル端末設定用の構造体変数
    speed_t speed = B115200;                        // 通信速度の設定

    ComFd=open(device, O_RDWR|O_NONBLOCK);   // シリアルポートのオープン

    if (ComFd < 0) {
        return -1;
    }
    tcgetattr(ComFd, &ComTio_Bk);           // 現在のシリアル端末設定状態を保存
    ComTio.c_iflag = 0;                     // シリアル入力設定の初期化
    ComTio.c_oflag = 0;                     // シリアル出力設定の初期化
    ComTio.c_cflag = CLOCAL|CREAD|CS8;      // シリアル制御設定の初期化
    ComTio.c_lflag = 0;                     // シリアルローカル設定の初期化
    bzero(ComTio.c_cc,sizeof(ComTio.c_cc)); // シリアル特殊文字設定の初期化
    cfsetispeed(&ComTio, speed);            // シリアル入力の通信速度の設定
    cfsetospeed(&ComTio, speed);            // シリアル出力の通信速度の設定
    ComTio.c_cc[VMIN] = 0;                  // リード待ち容量0バイト(待たない)
    ComTio.c_cc[VTIME] = 1;                 // リード待ち時間
    tcflush(ComFd,TCIFLUSH);                // バッファのクリア
    tcsetattr(ComFd, TCSANOW, &ComTio);     // シリアル端末に設定
    return ComFd;
}

char read_serial_port(void){
    char c='\0';                                    // シリアル受信した文字の代入用
    fd_set ComReadFds;                              // select命令用構造体ComReadFdを定義
    struct timeval tv;                              // タイムアウト値の保持用
    FD_ZERO(&ComReadFds);                           // ComReadFdの初期化
    FD_SET(ComFd, &ComReadFds);                     // ファイルディスクリプタを設定
    tv.tv_sec=0; tv.tv_usec=10000;                  // 受信のタイムアウト設定(10ms)
    if(select(ComFd+1, &ComReadFds, 0, 0, &tv)) read(ComFd, &c, 1); // データを受信
    usleep(5000);                                   // 5msの(IchigoJam処理)待ち時間
    return c;                                       // 戻り値＝受信データ(文字変数c)
}

int close_serial_port(void){
    tcsetattr(ComFd, TCSANOW, &ComTio_Bk);
    return close(ComFd);
}

int main(int argc,char **argv){
    int fd=-1,loop=1,i,com=0,ret,ver,ok,wait;
    time_t time_val,time_prev;
    char c;
    char s[256];
    struct termios uartattr;
    //                    0123456789ABC
    char UART_DEVICE[15]="/dev/ttyUSB10";

    printf("-------------------------------------\n");
    printf("Petit Ichigo Writer  by Wataru KUNINO\n");
    printf("-------------------------------------\n");
    printf("注意：ファームウェアを書き換えます。\n");
    printf("This software updates firmware on LPC\n");
    printf("-------------------------------------\n");
    
    if( argc == 3 ) TARGET_VERSION = atoi(argv[2]);
    if( argc >= 2){
        i=atoi(argv[1]);
        if( i>0 && i<= 64){
            snprintf(&UART_DEVICE[5],8,"ttyS%1d",i-1);  // Cygwin PC用
            fd = uart_open(UART_DEVICE, &uartattr);
            com=i;
        }
    }
    if(com==0) for(i=22;i>=0;i--){
        if(i>=20){
            snprintf(&UART_DEVICE[5],8,"rfcomm%1d",i-20);   // ポート探索(rfcomm0-2)
            com = i-20+1;
        }else if(i>=10){
            snprintf(&UART_DEVICE[5],8,"ttyUSB%1d",i-10);  // ポート探索(USB0～9)
            com = i-10+1;
        }else if(i==9) snprintf(&UART_DEVICE[5],8,"ttyAMA0");   // 拡張IOのUART端子に設定
        else{
            snprintf(&UART_DEVICE[5],8,"ttyS%1d",i);            // Cygwin PC用
            com=i+1;
        }
    //  log_date(UART_DEVICE);
        fd = uart_open(UART_DEVICE, &uartattr);
        if(fd >=0 ) break;
    }
    
    if (fd < 0){
        log_date("ERROR @ UART OPEN");
        return 1;
    }else{
        sprintf(s,"COM PORT = %s (%d)",UART_DEVICE,com);
        log_date(s);
    }
    if(close_serial_port()){            // 一旦閉じる
        log_date("ERROR @ UART CLOSE");
        return 1;
    }
    
    while(loop){
        sprintf(s,"lpc21isp/lpc21isp -verify -donotstart firmware/ichigojam-xtal.hex %s 115200 12000",UART_DEVICE);
        log_date(s);
        log_date("Please connect a LPC Device to your jig.");
        ret=system(s);
        sprintf(s,"lpc21isp (%d)",ret);         // 256:ファームウェア無し、2560 ユーザ終了 65280
        log_date(s);
        switch(ret){
            case 2560:
                log_date("[Esc]キーが押されたので書き換えを中断しました");
                return 1;
                break;
            case 0:
                log_date("SUCCESS");
                break;
            default:
                log_date("エラーが発生したので終了します");
                return 1;
                break;
        }

        log_date("Please press reset button on IchigoJam.");
        uart_open(UART_DEVICE, &uartattr);

        wait=0; ok=0; ver=0;
        while(ok>=0){
            if( kbhit() ){
                log_date("Aborted by user.");
                close_serial_port();
                return 1;
            }
            c=read_serial_port();
            if(c){
                if( wait > 3 ){
                    printf("\n");
                }
                wait =0;
                if( isprint((int)c) ) printf("%c",c);
                if( c =='\n' ){
                    printf("\n");
                    if(c =='\n' && ok == 2 && ver == TARGET_VERSION) ok=-1; // ループ終了
                }else if( isdigit((int)c) ){
                    ver *=10;
                    ver += (int)(c - '0');
                }else if( ver !=0 && ver != TARGET_VERSION && c !='.' ){
                    printf("\n");
                    log_date("IchigoJam Version ERROR.");
                    log_date("期待バージョンと異なるので終了します");
                    close_serial_port();
                    return 1;
                }else if( ok==0 && c=='O' ){
                    ok++;
                }else if( ok==1 && c=='K' ){
                    ok++;
                }else if( ok && isprint((int)c) ) ok=0;
            }
            time(&time_val);
            if( time_prev != time_val ){
                time_prev = time_val;
                wait++;
                if( wait>30 ){
                    printf("\n");
                    log_date("No response from IchigoJam.");
                    log_date("IchigoJamからの応答がありません");
                    close_serial_port();
                    return 1;
                }
                if( wait > 3) printf(".");
            }
        }
        if(close_serial_port()){
            log_date("ERROR @ UART CLOSE");
            return 1;
        }
        log_date("CLOSED");
    }
    return(0);
}


