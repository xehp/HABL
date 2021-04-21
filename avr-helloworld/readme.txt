



This is a program for AVR ATMEGA168 micro controllers.



Put "habl" on the atmega168
http://www.eit.se/habl/



# Install compiler and avrdude (on debian/ubuntu/mint style linux):
sudo apt-get install gcc-avr avr-libc binutils-avr avrdude cutecom lrzsz screen


This may also be handy (but not required)
apt-get install picocom 


compile:
make



http://unix.stackexchange.com/questions/56614/send-file-by-xmodem-or-kermit-protocol-with-gnu-screen

make sure to allow user to use dialout (or else you must run screen and cutecom as root) replace henrik below with your username.
sudo usermod -a -G dialout henrik


Connect to the device
screen /dev/ttyUSB0 9600
To leave Ctrl-A D
To and kill screen: Ctrl-A k y



Or picocom:
picocom --b 9600 /dev/ttyUSB0
To leave Ctrl-A Ctrl-X



When booting with HABL send 5 esc "1b 1b 1b 1b 1b" to habl within the first 3 seconds to enter X modem mode.
Habl will send  a 'C' once per second to confirm it is ready for xmodem.


Send the file "havrapp.bin" with cutecom xmodem

If cutecom xmodem did not work perhaps this is why:
https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=911378

Sending from command line instead:
stty -F /dev/ttyUSB0 9600 cs8 -parenb -cstopb -ixon
sx havrapp.bin < /dev/ttyUSB0 > /dev/ttyUSB0 



