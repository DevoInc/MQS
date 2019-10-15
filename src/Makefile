CC=gcc
EXTRAOPTS=-ggdb

all: mqs

mqs: main.c packet-loop.c mysql-decode.c
	$(CC) $^ $(EXTRAOPTS) -o mqs -Iinclude/ $(shell pkg-config luajit --cflags --libs) -lpcap 

clean:
	rm -f mqs
	rm -f *~
	rm -f include/*~
