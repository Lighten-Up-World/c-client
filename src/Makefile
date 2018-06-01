.PHONY: clean
.PHONY: build/

CFLAGS = -Wall -Werror -pedantic

RUN = add01

BUILD = ./build/
UTIL = ./utils/
EMULATE = ./emulate/

COMPILE = gcc -c $(CFLAGS) $< -o $@

emulate: build ./build/emulate.out
	$(BUILD)emulate.out ../test/test_cases/$(RUN)

$(BUILD)emulate.out: $(BUILD)emulate.o $(BUILD)io.o $(BUILD)execute.o $(BUILD)decode.o $(BUILD)register.o $(BUILD)bitops.o $(BUILD)error.o
	gcc $^ -o $@

dependencies:
	grep '#include' *.[ch] | grep '"'

$(BUILD)emulate.o: emulate.c $(UTIL)arm.h $(UTIL)io.h $(EMULATE)execute.h $(EMULATE)decode.h $(EMULATE)register.h
	$(COMPILE)

$(BUILD)io.o: $(UTIL)io.c $(UTIL)io.h $(UTIL)bitops.h
	$(COMPILE)

$(BUILD)execute.o: $(EMULATE)execute.c $(EMULATE)execute.h $(UTIL)bitops.h $(EMULATE)register.h
	$(COMPILE)

$(BUILD)decode.o: $(EMULATE)decode.c $(EMULATE)decode.h $(UTIL)arm.h $(UTIL)instructions.h $(UTIL)bitops.h
	$(COMPILE)

$(BUILD)register.o: $(EMULATE)register.c $(EMULATE)register.h $(UTIL)arm.h
	$(COMPILE)

$(BUILD)bitops.o: $(UTIL)bitops.c $(UTIL)bitops.h
	$(COMPILE)

$(BUILD)error.o: $(UTIL)error.c $(UTIL)error.h
	$(COMPILE)

build:
	mkdir -p build

clean:
	rm -rf $(BUILD)










