rm /opt/lampp/lampp_control
rm /opt/lampp/share/lampp_control/logo.gif
rm /opt/lampp/share/lampp_control/.license 2> /dev/null
rm /opt/lampp/share/locale/de/LC_MESSAGES/lampp_control.mo
rm /opt/lampp/share/lampp_control/uninstall.sh
rmdir /opt/lampp/share/lampp_control/
echo -n "Do you want remove the vhosts? [y]: "
read tmp
if [ "$tmp" == "" ]
then
    tmp="y"
fi

if [ "$tmp" == "y" ]
then
    echo -n "Removing Vhosts "
    rm /opt/lampp/etc/lampp_control-vhost.conf
    echo -n "."
    sed -e 's!Include etc/lampp_control-vhost.conf!!' /opt/lampp/etc/httpd.conf > /opt/lampp/etc/httpd.conf.tmp
    mv /opt/lampp/etc/httpd.conf.tmp /opt/lampp/etc/httpd.conf
    echo -n "."
    rm -rf /opt/lampp/etc/vhost_available
    echo -n "."
    rm -rf /opt/lampp/etc/vhost_enable
    echo "done."
    echo -n "Reloading Xampp ..."
    /opt/lampp/lampp reload > /dev/null
    echo "done."
fi

echo "Lampp Control is removed from this computer."
