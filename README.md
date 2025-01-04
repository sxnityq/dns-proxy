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
1) implement type option
2) fix hardcoded max number of argv
3) fix hardcoded code. Especially with ipv4\ipv6
4) adequate error messages
5) tolerant behaviour with long lexemes