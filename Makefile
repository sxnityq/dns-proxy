server: server.o
	gcc server.o -o server

server.o: server.c
	gcc -c server.c -o server.o

.PHONY: clean
clean:
	rm -f server && rm -f server.o
