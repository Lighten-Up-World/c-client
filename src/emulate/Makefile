.PHONY: clean
.PHONY: build/

CFLAGS = -Wall -Werror -pedantic

BUILD = ./build/
UTIL = ../utils/
SRC = ../

COMPILE = gcc $(CFLAGS) $< -o $@

emulate: ./build/emulate.out
	$(BUILD)emulate.out ../test/test_cases/

$(BUILD)emulate.out: $(BUILD)emulate.o $(BUILD)io.o $(BUILD)execute.o $(BUILD)decode.o $(BUILD)register.o $(BUILD)bitops.o $(BUILD)error.o
	gcc $(CFLAGS) $^ -o $(BUILD)emulate.out

dependencies:
	grep '#include' *.[ch] | grep '"'

$(BUILD)emulate.o: $(SRC)emulate.c $(UTIL)arm.h $(UTIL)io.h execute.h decode.h register.h
	$(COMPILE)

$(BUILD)io.o: $(UTIL)io.c $(UTIL)io.h $(UTIL)bitops.h
	$(COMPILE)

$(BUILD)execute.o: execute.c execute.h $(UTIL)bitops.h register.h
	$(COMPILE)

$(BUILD)decode.o: decode.c decode.h $(UTIL)arm.h $(UTIL)instructions.h $(UTIL)bitops.h
	$(COMPILE)

$(BUILD)register.o: register.c register.h $(UTIL)arm.h
	$(COMPILE)

$(BUILD)bitops.o: $(UTIL)bitops.c $(UTIL)bitops.h
	$(COMPILE)

$(BUILD)error.o: $(UTIL)error.c $(UTIL)error.h
	$(COMPILE)

build/:
	mkdir -p build

clean:
	rm -rf $(BUILD)










