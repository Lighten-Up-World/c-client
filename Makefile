.PHONY: test
.PHONY: unit_test
.PHONY: src

all: emulate unit_test test

test:
	cd ./test; make

emulate:
	cd src; make emulate DEBUG=

debug:
	cd src; make clean; make emulate

unit_test:
	cd src/test; make
