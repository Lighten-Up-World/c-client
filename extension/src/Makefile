.PHONY: clean
.PHONY: run
.PHONY: mkdir
.PHONY: simulation
.PHONY: server
.PHONY: layout
.PHONY: run
PATHROOT = ../../
PATHOPC = openpixelcontrol/
PATHLAYOUT = extension/src/layout/

PATHS = ./
PATHJSON = parson/
PATHUTIL = utils/
PATHEFFECT = effects/
PATHB = build/
PATHOBJ = build/objs/
PATHOPC = opc/

BUILD_PATHS = $(PATHB) $(PATHOBJ)

ROOT_O = $(patsubst $(PATHS)%.c,$(PATHOBJ)%.o,$(wildcard $(PATHS)*.c))
UTIL_O = $(patsubst $(PATHUTIL)%.c,$(PATHOBJ)%.o,$(wildcard $(PATHUTIL)*.c))
EFFECT_O = $(patsubst $(PATHEFFECT)%.c,$(PATHOBJ)%.o,$(wildcard $(PATHEFFECT)*.c))

COMPILE = gcc -c
LINK = gcc

DEBUG = -DDEBUG -g
CFLAGS = -Wall -Werror -pedantic -O0 -std=gnu99 $(DEBUG)

MODE = temp

simulation: mkdir server $(PATHB)effect_runner.out
	$(PATHB)effect_runner.out $(MODE)

conway: mkdir
	python $(PATHROOT)$(PATHOPC)python/conway.py

raverplaid: mkdir
	python $(PATHROOT)$(PATHOPC)python/raver_plaid.py

lavalamp: mkdir
	python $(PATHROOT)$(PATHOPC)python/lava_lamp.py --layout $(PATHROOT)$(PATHLAYOUT)WorldMap.json

server: layout
	cd $(PATHROOT)$(PATHOPC); make
	$(PATHROOT)$(PATHOPC)bin/gl_server -l $(PATHROOT)$(PATHLAYOUT)WorldMap.json &

layout:
	cd layout; make

run: mkdir $(PATHB)effect_runner.out
	./build/effect_runner.out $(MODE)

$(PATHB)effect_runner.out: $(ROOT_O) $(UTIL_O) $(EFFECT_O) $(PATHOBJ)opc_client.o $(PATHOBJ)parson.o
	$(LINK) -o $@ $^ -lm

$(PATHB)%.out: $(PATHOBJ)%.o
	$(LINK) -o $@ $^ -lm

$(PATHOBJ)%.o: $(PATHS)%.c
	$(COMPILE) $(CFLAGS) $< -o $@

$(PATHOBJ)%.o: $(PATHJSON)%.c $(PATHJSON)%.h
	$(COMPILE) $(CFLAGS) $< -o $@

$(PATHOBJ)%.o: $(PATHEFFECT)%.c $(PATHEFFECT)%.h
	$(COMPILE) $(CFLAGS) $< -o $@

$(PATHOBJ)%.o: $(PATHUTIL)%.c $(PATHUTIL)%.h
	$(COMPILE) $(CFLAGS) $< -o $@

$(PATHOBJ)%.o: $(PATHOPC)%.c $(PATHOPC)%.h
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
