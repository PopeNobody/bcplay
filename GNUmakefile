
MAKE_FLAGS:= -Rr --warn-undefined-variable $(shell cat etc/make_jobs_flag)

CONF_DEFS:=$(wildcard etc/*.def)
CONFS:=$(patsubst %.def,%,$(CONF_DEFS))
MISSING:=$(filter-out $(wildcard $(CONFS)), $(CONFS))

default: fake_tgt
	@mkdir -p log
	make -f Makefile 2>&1 $(MAKE_FLAGS) | tee log/xmake.out

%: fake_tgt
	@mkdir -p log
	make  -f Makefile $@ 2>&1 $(MAKE_FLAGS) | tee log/xmake.out

fake_tgt: $(CONFS)
	@echo starting make.

.PHONY: 

$(CONFS): %: %.def
	cp $< $@

GNUmakefile: ;
