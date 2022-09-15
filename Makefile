interface:	server
	cc	-o	interface	interface.c

server:	server.c
	cc	-o	server	server.c

clean:
	rm	-f	interface
	rm	-f	server
