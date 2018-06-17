CC = gcc
CFLAGS = -Wall -Werror -pedantic

main: dirs
	$(CC) $(CFLAGS) scroll_buffer.c main.c ../utils/list.c ../utils/csv.c  ../pixel.c -o ../build/scrolling/main.out
	../build/scrolling/main.out

debug:
	$(CC) $(CFLAGS) scroll_buffer.c main.c ../utils/list.c ../utils/csv.c ../pixel.c  -o ../build/scrolling/main.out -g

dirs:
	mkdir -p ../build/scrolling

clean:
	rm -f *.o
	rm -f *.out
	rm -f main
