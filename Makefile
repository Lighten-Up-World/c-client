.PHONY: test
.PHONY: unit_test
.PHONY: src

all: emulate assemble unit_test test

mac: emulate assemble unit_test test_mac

RUN = factorial

test:
	cd ./test; make

test_mac:
	cd ./test; make mac

assemble:
	cd src; make clean; make assemble DEBUG= RUN=$(RUN)

emulate:
	cd src; make clean; make emulate DEBUG= RUN=$(RUN)

debug:
	cd src; make clean; make emulate RUN=$(RUN)

unit_test:
	cd src/test; make

clean:
	cd src; make clean
