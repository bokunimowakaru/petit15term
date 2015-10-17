# Petit Ichigo Term for Linux / Raspberry Pi
IchigoJam用の簡易ターミナルソフトです。RaspberryPiからIchigoJamへアクセスすることが出来ます。
制御コードなどの対応は行っておりません。  

###動作確認済みUSBドライバ  
- FDTI FT231XS
- FDTI FT232RL
- Silicon Labo CP2102（ふうせん Fu-sen.様からの情報）
- Silicon Labo CP2104

###動作確認済みLinux OS
- Raspberry Pi : Linux version 3.1
- Ubuntu （ふうせん Fu-sen.様からの情報）
- Fedora （ふうせん Fu-sen.様からの情報）

###動作*しない*システム
- Cygwin for Windows : CYGWIN_NT-6.1 version 2.2.1  
  (/dev/ttyS?)

###Raspberry Piの拡張IOへの接続方法  
1. 画面左上の「MENU」から「Preferences」を選択し、「Raspberry Pi Configuration」を選択して設定画面を開きます。
1. タブ「Interfaces」の「Serial」を「Disable」に設定します（シリアル端末からのログインを無効にする設定）。
1. 設定後、「OK」をクリックすると再起動を促されるので再起動します。
1. IchigoJamをRaspberry Piの拡張IOに接続します。
      Pin 1 3.3V        Pin 2 5V  
      Pin 3 ----        Pin 4 ----  
      Pin 5 ----        Pin 6 GND  
      Pin 7 ----        Pin 8 UART TXD (to RXD on IchigoJam)  
      Pin 9 ----        Pin10 UART RXD (to RTD on IchigoJam)  
  
###関連ページ（当方のブログ）
- http://blogs.yahoo.co.jp/bokunimowakaru/55085520.html  
