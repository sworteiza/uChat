#CC=clang   time make -j
#CFLAGS=-std=c11 -Wall -Wextra -Werror -Wpedantic -Wdeprecated-declarations -Wincompatible-pointer-types
CC=gcc
CFLAGS=-O3 -Wall `pkg-config --cflags --libs gtk+-3.0`
LIBS=-pthread 
CLIENT = uchat
SERVER = uchat_server

#FLAGS = -I client/resource/include -L client/resource/lib -lgtk-3 -lgdk-3 -Wl,-framework,Cocoa -Wl,-framework,Carbon -Wl,-framework,CoreGraphics -lpangocairo-1.0 -lpango-1.0 -lharfbuzz -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0
all: $(CLIENT) $(SERVER)
#time reinstall
 
$(CLIENT):
	#$(CC) $(CFLAGS) $(LIBS) $(FLAGS) -o $(CLIENT) client/test.c
	$(CC) $(CFLAGS) $(LIBS) -o $(CLIENT) client/test.c
$(SERVER):
	$(CC) $(CFLAGS) $(LIBS) -lsqlite3 -o $(SERVER) server/*.c

.PHONY: clean 

unistall:
	rm -f *.o $(SERVER) $(CLIENT)

clean:
	rm -f uchat
	rm -f uchat_server

reinstall: unistall all 

	
	