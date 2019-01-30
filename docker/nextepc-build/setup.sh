#!/bin/sh

if ! grep "uptun" /proc/net/dev > /dev/null; then
    ip tuntap add name uptun mode tun
fi
ip addr del 45.45.0.1/16 dev uptun 2> /dev/null
ip addr add 45.45.0.1/16 dev uptun
ip addr del cafe::1/64 dev uptun 2> /dev/null
ip addr add cafe::1/64 dev uptun
ip link set uptun up
