all:
	echo "Bitte fuehren sie make install aus"
install: /opt/lampp/lampp_control /opt/lampp/logo.gif
/opt/lampp/lampp_control: lampp_control
	cp lampp_control /opt/lampp/lampp_control
/opt/lampp/logo.gif: logo.gif
	cp logo.gif /opt/lampp/logo.gif
