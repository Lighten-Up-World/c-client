CFLAGS = -Wall -Werror -pedantic -std=gnu99 -I/usr/local/Cellar//openssl/1.0.2o_2/include
LDLIBS =  -L/usr/local/Cellar//openssl/1.0.2o_2/lib/ -lssl -lcrypto

# $@ - The target of the rule.
# $< - The first prerequisite.
# $ˆ - All prerequisites.

server:
	gcc ctrlserver.c $(CFLAGS) $(LDLIBS) -o $@

clean:
	rm -f server

.PHONY: server
