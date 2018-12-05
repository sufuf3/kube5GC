if [ 1 -ne $# ]
then
    echo "You must specify argument : ./systemctl.sh  {start|stop|force-stop|restart|force-reload|status}"

    exit;
fi

systemctl $1 open5gc-hssd
systemctl $1 open5gc-pcrfd
systemctl $1 open5gc-amfd
systemctl $1 open5gc-upfd
systemctl $1 open5gc-smfd
