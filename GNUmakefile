
CF_DEFS:=$(wildcard etc/*.def)
CF_PRES:=$(wildcard $(patsubst %.def,%.mk,$(CF_DEFS)))
CF_MISS:=$(filter-out $(CF_PRES), $(patsubst %.def,%.mk,$(CF_DEFS)))


MAKE_FLAGS:= -Rr --warn-undefined-variable -j8

default: fake_tgt
	make -f Makefile 2>&1 $(MAKE_FLAGS) | tee log/xmake.out

%: fake_tgt
	make  -f Makefile $@ 2>&1 $(MAKE_FLAGS) | tee log/xmake.out

fake_tgt: $(CF_MISS)
	@echo starting make.

$(CF_MISS): %.mk: %.def
	cp $< $@

.PHONY: 
