# dns-proxy
! Test assignment for INANGO

## LIMITATIONS
1) the only blacklisted response type is refused. and it is hardcoded. I will implement it soon to allow blacklist types like: "not found" & "resolve to some static ipv4"
2) It is currently hardcoded to ipv4.
3) config file is hardcoded with path and name to config.ingo !!!!!!!

## QUICK INTROSUCTION
```
├── server - main file  
├── collors - shimer and shine shit. nothing special
├── llist -  functions for manage udp order
├── dns.h - contains some basic dns related struct
├── tools.h - assist functions like validate ipv4\domain etc
└── theory.txt - just my notes. Nothung special
```

## ABOUT CONFIG FILE

SYNTAX: 
<argument> <:> <argv>

master - set dns upstream server
blacklist - containes blacklisted domains. Currently the max amount is 16
type - format for response if domain is blacklisted (CURRENTLY NOT IMPLEMENTED)

EXAMPLE:
```
master:  8.8.8.8 
blacklist: www.youtube.com pornhub.com genshin.hoyoverse.com
type:   
```

## HOW TO TEST!
```
./server
echo "nameserver 127.0.0.1" > /etc/resolv.conf
chattr +i /etc/resolv.conf
dig -4 example.org
ping example.org
```

## TODO
1) implenet type option
2) fix hardcoded max number of argv
3) fix hardcoded code. Especially with ipv4\ipv6