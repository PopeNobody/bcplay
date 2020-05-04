
MAKE_FLAGS:= -Rr --warn-undefined-variable $(shell cat etc/make_jobs_flag)

CONF_DEFS:=$(wildcard etc/*.def)
CONFS:=$(patsubst %.def,%,$(CONF_DEFS))
MISSING:=$(filter-out $(wildcard $(CONFS)), $(CONFS))
$(foreach i, $(MISSING), $(shell cp $i.def $i))
$(shell mkdir -p log)
default: fake_tgt
	make -f Makefile 2>&1 $(MAKE_FLAGS) | tee log/xmake.out

%: fake_tgt
	make  -f Makefile $@ 2>&1 $(MAKE_FLAGS) | tee log/xmake.out

fake_tgt:
	@echo starting make.

.PHONY: 
