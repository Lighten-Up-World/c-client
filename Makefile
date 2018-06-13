.PHONY: test
.PHONY: unit_test
.PHONY: src

all: src unit_test test

mac: src unit_test test_mac

RUN = factorial

test:
	cd ./test; make

test_mac:
	cd ./test; make mac

assemble:
	cd src; make clean; make assemble DEBUG= RUN=$(RUN)

emulate:
	cd src; make clean; make emulate DEBUG= RUN=$(RUN)

memcheck: compile memcheck_as memcheck_em

memcheck_as: compile
	valgrind src/build/assemble.out test/test_cases/$(RUN).s src/build/out

memcheck_em: compile
	valgrind src/build/emulate.out test/test_cases/$(RUN)

compile:
	cd src; make clean; make compile

debug:
	cd src; make clean; make assemble RUN=$(RUN)
	xxd -c 4 src/out
	xxd -c 4 test/test_cases/$(RUN)

unit_test:
	cd src/test; make

clean:
	cd src; make clean
