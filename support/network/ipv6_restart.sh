#!/bin/sh

SYSTEM=`uname`;

if [ "$SYSTEM" = "Linux" ]; then
    if ! grep "uptun" /proc/net/dev > /dev/null; then
        ip tuntap add name uptun mode tun
    fi
    ip addr del 45.45.0.1/16 dev uptun 2> /dev/null
    ip addr add 45.45.0.1/16 dev uptun
    ip addr del cafe::1/64 dev uptun 2> /dev/null
    ip addr add cafe::1/64 dev uptun
    ip link set uptun up
    ip addr del fe80::2 dev lo 2> /dev/null
    ip addr del fe80::3 dev lo 2> /dev/null
    ip addr del fe80::4 dev lo 2> /dev/null
    ip addr del fe80::5 dev lo 2> /dev/null
    ip addr add fe80::2 dev lo
    ip addr add fe80::3 dev lo
    ip addr add fe80::4 dev lo
    ip addr add fe80::5 dev lo
else
    ifconfig lo0 alias 127.0.0.2 netmask 255.255.255.255
    ifconfig lo0 alias 127.0.0.3 netmask 255.255.255.255
    ifconfig lo0 alias 127.0.0.4 netmask 255.255.255.255
    ifconfig lo0 alias 127.0.0.5 netmask 255.255.255.255
    ifconfig lo0 inet6 delete fe80::2 prefixlen 128 2> /dev/null
    ifconfig lo0 inet6 delete fe80::3 prefixlen 128 2> /dev/null
    ifconfig lo0 inet6 delete fe80::4 prefixlen 128 2> /dev/null
    ifconfig lo0 inet6 delete fe80::5 prefixlen 128 2> /dev/null
    ifconfig lo0 inet6 add fe80::2 prefixlen 128
    ifconfig lo0 inet6 add fe80::3 prefixlen 128
    ifconfig lo0 inet6 add fe80::4 prefixlen 128
    ifconfig lo0 inet6 add fe80::5 prefixlen 128
fi
