.PHONY: clean
.PHONY: emulate_production
.PHONY: emulate_debug
.PHONY: production
.PHONY: debug
.PHONY: build/
.ONESHELL:

CFLAGS = -Wall -Werror -pedantic
EMULATE_SOURCES = ./emulate/*.c ./emulate/*.h
UTIL_SOURCES = ./utils/*.h ./utils/*.c
OBJECTS = $(EMULATE_SOURCES:.c=.o) $(UTIL_SOURCES:.c=.o)
EXECUTABLE = emulate.out
COMPILE = gcc $(CFLAGS) $< -o $@

BUILD = ./build/

emulate: $(EMULATE_SOURCES) $(UTIL_SOURCES) $(EXECUTABLE)
	$(BUILD)$(EXECUTABLE) /test/test_cases/

$(EXECUTABLE): $(OBJECTS)
	$(COMPILE)

dependencies:
	grep '#include' *.[ch] | grep '"'

$(BUILD)emulate.o: ./emulate/emulate.c ./utils/arm.h ./utils/io.h ./emulate/execute.h ./emulate/decode.h ./emulate/register.h
	$(COMPILE)

$(BUILD)io.o: ./utils/io.c ./utils/io.h ./utils/bitops.h
	$(COMPILE)

$(BUILD)execute.o: ./emulate/execute.c ./emulate/execute.h ./utils/bitops.h ./emulate/register.h
	$(COMPILE)

$(BUILD)decode.o: ./emulate/decode.c ./emulate/decode.h ./utils/arm.h ./utils/instructions.h ./utils/bitops.h
	$(COMPILE)

$(BUILD)register.o: ./emulate/register.c ./emulate/register.h ./utils/arm.h
	$(COMPILE)

$(BUILD)bitops.o: ./utils/bitops.c ./utils/bitops.h
	$(COMPILE)

$(BUILD)error.o: ./utils/error.c ./utils/error.h
	$(COMPILE)

build/:
	mkdir -p build

clean:
	rm -rf $(BUILD)

# PATHS = ./

# PATHO = build/objs/
#
# BUILD_PATHS = $(PATHB) $(PATHO)
#
# SRC = $(wildcard $(PATHS)*.c)
#
# RESULTS = $(patsubst $(PATHS)%.c, $(SRC))
#
# CFLAGS=-I. -I$(PATHS) -DTEST -Wall -Werror -pedantic
#
# all: emulate assemble
#
# emulate: $(PATHB)emulate.out
#
# assemble: $(PATHB)assemble.out
#
# $(PATHB)%.out: $(PATHO)%.o
# 	gcc -o $@ $^
#
# $(PATHO)%.o: $(PATHS)%.c $(PATHS)%.h
# 	gcc -c $(CFLAGS) $< -o $@
#
# $(PATHB):
# 	mkdir -p $(PATHB)
#
# $(PATHO):
# 	mkdir -p $(PATHO)
#

# .PRECIOUS: $(PATHB)%.out
# .PRECIOUS: $(PATHD)%.d
# .PRECIOUS: $(PATHO)%.o
