# Petit Ichigo Term for Cygwin / Raspberry Pi
IchigoJam用の簡易ターミナルソフトです。RaspberryPiからIchigoJamへアクセスすることが出来ます。
制御コードなどの対応は行っておりません。  
本ページの後半にIchigoJamのファームウェア書き込みソフトも紹介しています。

###動作確認済みUSBドライバ  
- FDTI FT231XS
- FDTI FT232RL
- Silicon Labo CP2102（ふうせん Fu-sen.様からの情報）
- Silicon Labo CP2104

###動作確認済みLinux OS
- Raspberry Pi : Linux version 3.1
- Cygwin for Windows
- Ubuntu （ふうせん Fu-sen.様からの情報）
- Fedora （ふうせん Fu-sen.様からの情報）

###Raspberry PiやWindows PCのUSBへの接続方法  
1. IchigoJam用プリント基板「Personal computer※」のCN7にUSBシリアルアダプタを接続します。  
      ※CQ出版社「IchigoJam用コンピュータ電子工作学習キット(IF ICH-KIT)」に含まれる基板です
2. USBケーブルを使ってRaspberry Piへ接続します。
3. 電源スイッチSW1をDC IN側にします（ACアダプタは不要）。
4. マイコンの右側の端子だけをソケットCN3に接続して電源を入れます。

###Raspberry Piの拡張IOへの接続方法  
1. Raspberry Piの画面左上の「MENU」から「Preferences」を選択し、「Raspberry Pi Configuration」を選択して設定画面を開きます。
2. タブ「Interfaces」の「Serial」を「Disable」に設定します（シリアル端末からのログインを無効にする設定）。
3. 設定後、「OK」をクリックすると再起動を促されるので再起動します。
4. IchigoJamをRaspberry Piの拡張IO（下表）に接続します。  
      Pin 1 3.3V        Pin 2 5V  
      Pin 3 ----        Pin 4 ----  
      Pin 5 ----        Pin 6 GND  
      Pin 7 ----        Pin 8 UART TXD (to RXD on IchigoJam)  
      Pin 9 ----        Pin10 UART RXD (to RTD on IchigoJam)  
5. IchigoJam用プリント基板「Personal computer※」の場合は、CN7を利用すると良いでしょう。

###必要な機器
- Cygwin / Raspberry Piが動作する環境
- IchigoJam
- USBシリアル変換アダプタ (またはジャンパワイヤでRaspPiのUARTに接続)

###使用方法
- cd petit15term でディレクトリを変更します。
- make clean all でコンパイルを行います。
- ./petit15term で実行します。

###関連ページ（当方のブログ）
①ジャンパー線で接続する（配線図を追加しました）
- http://blogs.yahoo.co.jp/bokunimowakaru/55078094.html

②USBで接続する
- http://blogs.yahoo.co.jp/bokunimowakaru/55085520.html

③Bluetoothで接続する
- http://blogs.yahoo.co.jp/bokunimowakaru/55201268.html

------------------------------------------------------------------------
# Petit Ichigo Writer for Cygwin / Raspberry Pi
IchigoJam用のファームウェアを連続で書き込むためのソフトウェアです。
CygwinまたはRaspberry Piで動作します。
ファームウェアは下記からダウンロードしてください。
- http://ichigojam.net/farm.html

ご注意：ファームウェアの利用にはjig.jpのライセンスへの同意が必要です。
販売目的でファームウェアを使用する場合はライセンス料の支払いが必要です。

###必要な機器
- Windowsが動作するパソコンまたはRaspberry Pi
- IchigoJamプリント基板
　(リセットとISPボタン、USBシリアル変換アダプタが必要です)  
　CQ出版社「IchigoJam用コンピュータ電子工作学習キット(IF ICH-KIT)」に含まれるPersonal Computer基板が便利。

###使用方法
- firmwareフォルダ内にファームウェア「ichigojam-xtal.hex」を保存してください。
- Cygwinを起動してください（Raspberry Piの場合はLXTerminalを起動）
- git clone https://github.com/bokunimowakaru/petit15term.git でダウンロードします。
- cd petit15term でディレクトリを変更します。
- make clean all でコンパイルを行います。
- ./petit15writer で実行します。

実行時に数字を付与するとWindows PCのCOMポート番号を指定することが出来ます。
- ./petit15writer 5 でCOM5を使用します

ファームウェアのバージョン確認が1.1.1の時だけ、次の処理に移ります。
指定バージョンを変更するには、実行時の第2引数に3桁の数字を付与します。
- ./petit15writer 0 121 で「121」すなわちバージョン1.2.1を指定することができます。
- ./petit15writer 5 121 のようにCOMポートとバージョンの両方を指定することも出来ます。

以下に使用時の様子を示します。

![alt text](https://github.com/bokunimowakaru/petit15term/blob/master/README.jpg)

------------------------------------------------------------------------
書き込むマイコンの数量が少ない場合は、FlashMagicの方が便利です。連続して書き込む場合は本ソフトが便利でしょう。あまり使う人はいないと思うので、バグ以外のサポートはいたしません。また、補償もいたしません。例えば、lpc21ispにベリファイ機能があり、それを検出しているはずですが、本当にエラーになるかどうかは試していません。
