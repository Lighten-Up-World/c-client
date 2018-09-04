CFLAGS = -Wall -Werror -pedantic -std=gnu99 -I/usr/local/Cellar//openssl/1.0.2o_2/include
LDLIBS =  -L/usr/local/Cellar//openssl/1.0.2o_2/lib/ -lssl -lcrypto

# $@ - The target of the rule.
# $< - The first prerequisite.
# $ˆ - All prerequisites.

server: ctrlserver.c ctrlserver.h
	gcc ctrlserver.c $(CFLAGS) $(LDLIBS) -o $@

test: tests/test_hash.c
	gcc $< $(CFLAGS) $(LDLIBS) -o $@

clean:
	rm -f server test

.PHONY: server
.PHONY: sha
