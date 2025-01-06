# dns-proxy
! Test assignment for INANGO

## LIMITATIONS
1) the only blacklisted response type is refused. and it is hardcoded. I will implement it soon to allow blacklist types like: "not found" & "resolve to some static ipv4"
2) It is currently hardcoded to ipv4.


## ABOUT CONFIG FILE

SYNTAX: 

MASTER: ipv4 \n

BAN: [ domain1 domain2 ... domainN ] \n


master - set dns upstream server

blacklist - containes blacklisted domains. Currently the max amount is 16

!TODO type - format for response if domain is blacklisted (CURRENTLY NOT IMPLEMENTED)

EXAMPLE:
```
MASTER: 8.8.8.8
BAN: [ temu.com. genshin.hoyoverse.com. arestovich.com. ]
```

## HOW TO TEST!
```
make
sudo ./main
echo "nameserver 127.0.0.1" > /etc/resolv.conf
chattr +i /etc/resolv.conf
dig -4 arestovich.com
ping arestovich.com
```

## TODO
1) tolerant behaviour with long lexemes
2) implement type option
3) ADD ipv6 support
4) systemd-unit
5) AUR package
