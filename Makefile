.PHONY: test
.PHONY: unit_test
.PHONY: src

all: emulate unit_test test

RUN = factorial

test:
	cd ./test; make

emulate:
	cd src; make clean; make emulate DEBUG= RUN=$(RUN)

debug:
	cd src; make clean; make emulate RUN=$(RUN)

unit_test:
	cd src/test; make

clean:
	cd src; make clean
