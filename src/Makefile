.PHONY: clean
.PHONY: run
.PHONY: mkdir
.PHONY: simulation
.PHONY: server
.PHONY: layout
.PHONY: run
PATHROOT = ../
PATHOPC = openpixelcontrol/
PATHGAMES = games/
PATHLAYOUT = src/layout/

PATHS = ./
PATHJSON = parson/
PATHUTIL = utils/
PATHEFFECT = effects/
PATHB = build/
PATHOBJ = build/objs/
PATHOPENPC = opc/

BUILD_PATHS = $(PATHB) $(PATHOBJ)

ROOT_O = $(patsubst $(PATHS)%.c,$(PATHOBJ)%.o,$(wildcard $(PATHS)*.c))
UTIL_O = $(patsubst $(PATHUTIL)%.c,$(PATHOBJ)%.o,$(wildcard $(PATHUTIL)*.c))
EFFECT_O = $(patsubst $(PATHEFFECT)%.c,$(PATHOBJ)%.o,$(wildcard $(PATHEFFECT)*.c))

COMPILE = gcc -c
LINK = gcc -lssl -lcrypto

DEBUG = -DDEBUG -g
CFLAGS = -Wall -Werror -pedantic -O0 -std=gnu99 $(DEBUG)

MODE = temp

simulation: mkdir server $(PATHB)effect_runner.out
	$(PATHB)effect_runner.out $(MODE)

demo: mkdir $(PATHB)effect_runner.out
	$(PATHS)demo.sh

conway: mkdir
	python $(PATHROOT)$(PATHOPC)python/conway.py

raverplaid: mkdir
	python $(PATHROOT)$(PATHOPC)python/raver_plaid.py

lavalamp: mkdir
	python $(PATHROOT)$(PATHOPC)python/lava_lamp.py --layout $(PATHROOT)$(PATHLAYOUT)WorldMap.json

snake: mkdir
	python $(PATHROOT)$(PATHGAMES)snake/snake.py

server: layout
	cd $(PATHROOT)$(PATHOPC); make
	$(PATHROOT)$(PATHOPC)bin/gl_server -l $(PATHROOT)$(PATHLAYOUT)WorldMap.json &

1by1:
	python ../../fadecandy/examples/python/one-by-one.oy

old_layout:
	cp layout/csv/WorldMap_old.csv layout/csv/WorldMap.csv
	python layout/csv_to_layout.py

new_layout:
	cp layout/csv/WorldMap_new.csv layout/csv/WorldMap.csv
	python layout/csv_to_layout.py

run_bones: mkdir $(PATHB)bare_bones_test.out
	./build/bare_bones_test.out

black:
	python ../../fadecandy/examples/python/solid-black.py


run: mkdir $(PATHB)effect_runner.out
	./build/effect_runner.out $(MODE)

$(PATHB)ctrlserver.out: $(PATHOBJ)ctrlserver.o $(PATHOBJ)pixel.o $(PATHOBJ)list.o $(PATHOBJ)opc_client.o
	$(LINK) $(LDLIBS) $(SERVERFLAGS)-o $@ $^ -lm

$(PATHB)bare_bones_test.out: $(PATHOBJ)bare_bones_test.o $(PATHOBJ)pixel.o $(PATHOBJ)list.o $(PATHOBJ)opc_client.o
	$(LINK) -o $@ $^ -lm

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

$(PATHOBJ)%.o: $(PATHOPENPC)%.c $(PATHOPENPC)%.h
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
