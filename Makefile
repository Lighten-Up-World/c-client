.PHONY: test
.PHONY: unit_test
.PHONY: src

all: src unit_test test

test:
	cd ./test; make

emulate:
	cd src; make emulate

src:
	cd src; make clean; make emulate DEBUG=

unit_test:
	cd src/test; make
