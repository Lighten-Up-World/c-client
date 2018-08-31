CFLAGS = -Wall -Werror -pedantic -O0 -std=gnu99
CRYPT = -I/usr/local/Cellar//openssl/1.0.2o_2/include -L/usr/local/Cellar//openssl/1.0.2o_2/lib/ -lssl -lcrypto

server:
	gcc ctrlserver.c $(CFLAGS) $(CRYPT) -o ctrl

sha:
	gcc sha.c $(CFLAGS) $(CRYPT) -o sha

clean:
	rm -f sha
	rm -f ctrl