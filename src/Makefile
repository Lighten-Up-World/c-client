



.PHONY: clean
.PHONY: all
.PHONY: emulate
.PHONY: assemble

BUILD = ./build/

CFLAGS = -Wall -Werror -pedantic

COMPILE = gcc -c $(CFLAGS) $< -o $@

emulate: $(BUILD) emulate.out
	./build/emulate.out ../test/test_cases/beq01

dependencies:
	grep '#include' *.[ch] | grep '"'

emulate.out: $(BUILD)emulate.o $(BUILD)io.o $(BUILD)execute.o $(BUILD)decode.o $(BUILD)register.o $(BUILD)bitops.o $(BUILD)error.o
	gcc $(CFLAGS) $^ -o $(BUILD)$@

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

$(BUILD):
	mkdir -p $(BUILD)

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
# clean:
# 	rm -f $(PATHO)*.o
# 	rm -f $(PATHB)*.out
#
# .PRECIOUS: $(PATHB)%.out
# .PRECIOUS: $(PATHD)%.d
# .PRECIOUS: $(PATHO)%.o
