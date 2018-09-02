CFLAGS = -Wall -Werror -pedantic -std=gnu99 -I/usr/local/Cellar//openssl/1.0.2o_2/include
LDLIBS =  -L/usr/local/Cellar//openssl/1.0.2o_2/lib/ -lssl -lcrypto

# $@ - The target of the rule.
# $< - The first prerequisite.
# $ˆ - All prerequisites.

server: ctrl.o crypto.o
	gcc $(CFLAGS) $(LDLIBS) $^ -o $@

ctrl.o: ctrlserver.c ctrlserver.h server_crypto.h
	gcc $(CFLAGS) $(CRYPT) $< -o $@

crypto.o: server_crypto.c server_crypto.h
	gcc $(CFLAGS) $(CRYPT) $< -o $@

sha:
	gcc sha.c $(CFLAGS) $(CRYPT) -o sha1

clean:
	rm -f sha ctrl crypt

.PHONY: clean
.PHONY: server
.PHONY: sha
.PHONY: crypt