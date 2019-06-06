CC = g++
CFLAGS = -Wall -ansi 
# Wall: Show warning messages
# ansi: Enhance portability

all: page.c
	$(CC) -o page page.c $(CFLAGS)

clean:
	rm page
