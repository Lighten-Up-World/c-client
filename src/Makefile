.PHONY: clean
.PHONY: build/
.PHONY: emulate
.PHONY: assemble
	
DEBUG = -DDEBUG
CFLAGS = -Wall -Werror -pedantic -g -O0 $(DEBUG)

RUN = add01
OUT = out

BUILD = ./build/
UTIL = ./utils/
EMULATE = ./emulate/
ASSEMBLE = ./assemble/

COMPILE = gcc -c $(CFLAGS) $< -o $@
all: assemble emulate

debug_ass:
	make clean; make assemble RUN=$(RUN)

assemble: build ./build/assemble.out
	$(BUILD)assemble.out ../test/test_cases/$(RUN).s $(OUT)

$(BUILD)assemble.out: $(BUILD)assemble.o $(BUILD)io.o $(BUILD)encode.o $(BUILD)parser.o $(BUILD)tokenizer.o $(BUILD)bitops.o $(BUILD)error.o $(BUILD)symbolmap.o $(BUILD)referencemap.o $(BUILD)register.o
	gcc $^ -o $@

$(BUILD)assemble.o: assemble.c assemble.h
	$(COMPILE)

$(BUILD)encode.o: $(ASSEMBLE)encode.c $(ASSEMBLE)encode.h
	$(COMPILE)

$(BUILD)parser.o: $(ASSEMBLE)parser.c $(ASSEMBLE)parser.h
	$(COMPILE)

$(BUILD)tokenizer.o: $(ASSEMBLE)tokenizer.c $(ASSEMBLE)tokenizer.h
	$(COMPILE)

$(BUILD)symbolmap.o: $(ASSEMBLE)symbolmap.c $(ASSEMBLE)symbolmap.h
	$(COMPILE)

$(BUILD)referencemap.o: $(ASSEMBLE)referencemap.c $(ASSEMBLE)referencemap.h
	$(COMPILE)

emulate: build ./build/emulate.out
	$(BUILD)emulate.out ../test/test_cases/$(RUN)

$(BUILD)emulate.out: $(BUILD)emulate.o $(BUILD)io.o $(BUILD)execute.o $(BUILD)decode.o $(BUILD)register.o $(BUILD)bitops.o $(BUILD)error.o
	gcc $^ -o $@

$(BUILD)emulate.o: emulate.c $(UTIL)arm.h $(UTIL)io.h $(EMULATE)execute.h $(EMULATE)decode.h $(UTIL)register.h
	$(COMPILE)

$(BUILD)execute.o: $(EMULATE)execute.c $(EMULATE)execute.h $(UTIL)bitops.h $(UTIL)register.h
	$(COMPILE)

$(BUILD)decode.o: $(EMULATE)decode.c $(EMULATE)decode.h $(UTIL)arm.h $(UTIL)instructions.h $(UTIL)bitops.h
	$(COMPILE)

# UTILITY OBJECTS

$(BUILD)io.o: $(UTIL)io.c $(UTIL)io.h $(UTIL)bitops.h $(UTIL)register.h
	$(COMPILE)

$(BUILD)bitops.o: $(UTIL)bitops.c $(UTIL)bitops.h
	$(COMPILE)

$(BUILD)error.o: $(UTIL)error.c $(UTIL)error.h
	$(COMPILE)

$(BUILD)register.o: $(UTIL)register.c $(UTIL)register.h $(UTIL)arm.h
	$(COMPILE)

dependencies:
	grep '#include' *.[ch] | grep '"'

build:
	mkdir -p build

clean:
	rm -rf $(BUILD)
