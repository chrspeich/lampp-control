all: lampp_control
lampp_control: lampp_control.c
	gcc -o lampp_control lampp_control.c `pkg-config gtk+-2.0 --cflags --libs`
install: lampp_control /opt/lampp/ logo.gif
	cp lampp_control /opt/lampp/lampp_control
	cp logo.gif /opt/lampp/logo.gif
