.PHONY:all
all:server client

server:server.c com.c
	gcc -std=c99 $^ -o $@

client:client.c com.c
	gcc -std=c99 $^ -o $@

.PHONY:clean
clean:
	rm -rf server client
