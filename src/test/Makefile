.PHONY: clean
.PHONY: test


PATHU = unity/
PATHS = ../
PATHUT = ../utils/
PATHEM = ../emulate/
PATHAS = ../assemble/
PATHT = ./
PATHB = build/
PATHD = build/depends/
PATHO = build/objs/
PATHR = build/results/
PATHRUN = build/runner/

BUILD_PATHS = $(PATHB) $(PATHD) $(PATHO) $(PATHR) $(PATHRUN)

SRCT = $(wildcard $(PATHT)*.c)
SRCRUN = $(wildcard $(PATHRUN)*.c)

COMPILE=gcc -c
LINK=gcc
DEPEND=gcc -MM -MG -MF
CFLAGS=-I. -I$(PATHU) -I$(PATHS) -I$(PATHUT) -I$(PATHEM) -I$(PATHAS) -DTEST

RESULTS = $(patsubst $(PATHT)test_%.c,$(PATHR)test_%.txt,$(SRCT) )

PASSED = `grep -s PASS $(PATHR)*.txt`
FAIL = `grep -s FAIL $(PATHR)*.txt`
IGNORE = `grep -s IGNORE $(PATHR)*.txt`

test: $(BUILD_PATHS) $(RESULTS)
	@echo "$(SRCT)"
	@echo "-----------------------\nIGNORES:\n-----------------------"
	@echo "$(IGNORE)"
	@echo "-----------------------\nFAILURES:\n-----------------------"
	@echo "$(FAIL)"
	@echo "-----------------------\nPASSED:\n-----------------------"
	@echo "$(PASSED)"
	@echo "\nDONE"

$(PATHR)%.txt: $(PATHB)%.out
	-./$< > $@ 2>&1

$(PATHB)test_%.out: $(PATHO)test_%_run.o  $(PATHO)test_%.o $(PATHO)%.o $(PATHO)unity.o
	$(LINK) -o $@ $^

$(PATHO)%.o:: $(PATHT)%.c
	$(COMPILE) $(CFLAGS) $< -o $@

$(PATHO)%_run.o:: $(PATHT)%.c
	ruby $(PATHU)generate_test_runner.rb $< $(PATHRUN)$(*F).c
	$(COMPILE) $(CFLAGS) $(PATHRUN)$(*F).c -o $@

$(PATHO)%.o:: $(PATHS)%.c
	$(COMPILE) $(CFLAGS) $< -o $@

$(PATHO)%.o:: $(PATHUT)%.c
	$(COMPILE) $(CFLAGS) $< -o $@

$(PATHO)%.o:: $(PATHEM)%.c
	$(COMPILE) $(CFLAGS) $< -o $@

$(PATHO)%.o:: $(PATHAS)%.c
	$(COMPILE) $(CFLAGS) $< -o $@

$(PATHO)%.o:: $(PATHU)%.c $(PATHU)%.h
	$(COMPILE) $(CFLAGS) $< -o $@

$(PATHD)%.d:: $(PATHT)%.c
	$(DEPEND) $@ $<

$(PATHB):
	mkdir -p $(PATHB)

$(PATHD):
	mkdir -p $(PATHD)

$(PATHO):
	mkdir -p $(PATHO)

$(PATHR):
	mkdir -p $(PATHR)

$(PATHRUN):
	mkdir -p $(PATHRUN)

clean:
	rm -f $(PATHO)*.o
	rm -f $(PATHB)*.out
	rm -f $(PATHR)*.txt

.PRECIOUS: $(PATHB)test_%.out
.PRECIOUS: $(PATHO)%.o
.PRECIOUS: $(PATHR)%.txt
