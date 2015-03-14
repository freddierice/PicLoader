#!/bin/bash

if [ $# -eq 1 ]; then
    echo Pick a man in the middle account!
    echo "$0" "<mim acct>"
fi

mim=$1

HTTP_PORT=80
HTTPS_PORT=443

RED_HTTP_PORT=9090
RED_HTTPS_PORT=9091

USER=anon

# reset all rules
iptables -F 
iptables -F -t nat

# reroute all http traffic to the mim user
iptables -t nat -A OUTPUT -m owner --uid-owner $USER -p tcp --dport $HTTP_PORT -j REDIRECT --to-port $RED_HTTP_PORT

# reroute all http traffic to the mim user
iptables -t nat -A OUTPUT -m owner --uid-owner $USER -p tcp --dport $HTTPS_PORT -j REDIRECT --to-port $RED_HTTPS_PORT
