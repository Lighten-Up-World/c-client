.PHONY: test
.PHONY: unit_test
.PHONY: compile

all: compile unit_test test

mac: compile unit_test test_mac

RUN = factorial
RESO = low
VALGRIND_MEM = --tool=memcheck --leak-check=full --show-reachable=yes --num-callers=20 --track-fds=yes --track-origins=yes
VALGRIND_CALL = --tool=callgrind
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
	valgrind $(VALGRIND_MEM) src/build/assemble.out test/test_cases/$(RUN).s src/build/out

memcheck_em: compile
	valgrind $(VALGRIND_MEM) src/build/emulate.out test/test_cases/$(RUN)

compile:
	cd src; make clean; make compile DEBUG= 

debug:
	cd src; make clean; make assemble RUN=$(RUN)
	xxd -c 4 src/out
	xxd -c 4 test/test_cases/$(RUN)

unit_test:
	cd src/unit_tests; make

clean:
	cd src; make clean
