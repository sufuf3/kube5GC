if [ 1 -ne $# ]
then
    echo "You must specify argument : ./systemctl.sh  {start|stop|force-stop|restart|force-reload|status}"

    exit;
fi

systemctl $1 nextepc-hssd
systemctl $1 nextepc-pcrfd
systemctl $1 free5gc-amfd
systemctl $1 free5gc-upfd
systemctl $1 free5gc-smfd
