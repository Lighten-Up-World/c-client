CFLAGS = -Wall -Werror -pedantic -std=gnu99
LDLIBS = -lssl -lcrypto

# $@ - The target of the rule.
# $< - The first prerequisite.
# $ˆ - All prerequisites.

mac: ctrlserver.c ctrlserver.h
	gcc ctrlserver.c $(CFLAGS) $(LDLIBS) -o server

pi: ctrlserver.c ctrlserver.h
	gcc ctrlserver.c $(CFLAGS) -lcrypto -o server

test: tests/test_hash.c
	gcc $< $(CFLAGS) $(LDLIBS) -o $@

clean:
	rm -f server test

.PHONY: server
.PHONY: sha
