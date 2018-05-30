.PHONY: clean
.PHONY: emulate_production
.PHONY: emulate_debug
.PHONY: production
.PHONY: debug

.ONESHELL:

BUILD = ./build/

CFLAGS = -Wall -Werror -pedantic
DEBUG = -DDEBUG
RUN = factorial
COMPILE = gcc -c $(DEBUG) $(CFLAGS) $< -o $@

emulate: build/ emulate.out
	$(BUILD)emulate.out ../test/test_cases/$(RUN)

emulate.out: $(BUILD)emulate.o $(BUILD)io.o $(BUILD)execute.o $(BUILD)decode.o $(BUILD)register.o $(BUILD)bitops.o $(BUILD)error.o
	gcc $(CFLAGS) $^ -o $(BUILD)emulate.out

dependencies:
	grep '#include' *.[ch] | grep '"'

$(BUILD)emulate.o: emulate.c arm.h io.h execute.h decode.h register.h
	$(COMPILE)

$(BUILD)io.o: io.c io.h bitops.h
	$(COMPILE)

$(BUILD)execute.o: execute.c execute.h bitops.h register.h
	$(COMPILE)

$(BUILD)decode.o: decode.c decode.h arm.h instructions.h bitops.h
	$(COMPILE)

$(BUILD)register.o: register.c register.h arm.h
	$(COMPILE)

$(BUILD)bitops.o: bitops.c bitops.h
	$(COMPILE)

$(BUILD)error.o: error.c error.h
	$(COMPILE)

build/:
	mkdir -p $(BUILD)

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
