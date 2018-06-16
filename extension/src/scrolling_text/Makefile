CC = gcc
CFLAGS = -Wall -Werror -pedantic

main:
	$(CC) $(CFLAGS) scroll_buffer.c main.c -o main.out
	./main.out

debug:
	$(CC) $(CFLAGS) scroll_buffer.c main.c -o main -g

clean:
	rm -f *.o
	rm -f *.out
	rm -f main
