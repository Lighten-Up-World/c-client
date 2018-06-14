.PHONY: clean
.PHONY: run
.PHONY: mkdir

PATHS = ./
PATHJSON = parson/
PATHB = build/
PATHOBJ = build/objs/

BUILD_PATHS = $(PATHB) $(PATHOBJ)

SRCT = $(wildcard $(PATHT)*.c)
SRCRUN = $(wildcard $(PATHRUN)*.c)

COMPILE=gcc -c
LINK=gcc

DEBUG = -DDEBUG
CFLAGS = -Wall -Werror -pedantic -g -O0 $(DEBUG)

simulation: mkdir control $(PATHB)main.out
	cd simulation; make

control:
	cd ../../openpixelcontrol; make

run: mkdir $(PATHB)main.out
	./build/main.out

$(PATHB)main.out: $(PATHOBJ)main.o $(PATHOBJ)apimanager.o $(PATHOBJ)parson.o
	$(LINK) -o $@ $^

$(PATHB)%.out: $(PATHOBJ)%.o
	$(LINK) -o $@ $^

$(PATHOBJ)%.o: $(PATHS)%.c
	$(COMPILE) $(CFLAGS) $< -o $@

$(PATHOBJ)%.o: $(PATHJSON)%.c $(PATHJSON)%.h
	$(COMPILE) $(CFLAGS) $< -o $@

mkdir: $(PATHB) $(PATHOBJ)

$(PATHB):
	mkdir -p $(PATHB)

$(PATHOBJ):
	mkdir -p $(PATHOBJ)

clean:
	rm -f $(PATHOBJ)*.o
	rm -f $(PATHB)*.out

.PRECIOUS: $(PATHB)test_%.out
.PRECIOUS: $(PATHOBJ)%.o
