.PHONY:all
all:server client

server:server.c com.c
	gcc $^ -o $@

client:client.c com.c
	gcc $^ -o $@

.PHONY:clean
clean:
	rm -rf server client
