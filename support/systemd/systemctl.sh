if [ 1 -ne $# ]
then
    echo "You must specify argument : ./systemctl.sh  {start|stop|force-stop|restart|force-reload|status}"

    exit;
fi

systemctl $1 nextepc-mmed
systemctl $1 nextepc-hssd
systemctl $1 nextepc-pcrfd
systemctl $1 nextepc-upfd
systemctl $1 nextepc-smfd
