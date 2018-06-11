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

src:
	cd src; make clean; make all DEBUG= RUN=$(RUN)

assemble:
	cd src; make clean; make assemble DEBUG= RUN=$(RUN)

emulate:
	cd src; make clean; make emulate DEBUG= RUN=$(RUN)

debug:
	cd src; make clean; make assemble RUN=$(RUN)

unit_test:
	cd src/test; make

clean:
	cd src; make clean
