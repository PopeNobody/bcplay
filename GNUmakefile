
MAKE_FLAGS:= -Rr --warn-undefined-variable $(shell cat etc/make_jobs_flag)

default: fake_tgt
	make -f Makefile 2>&1 $(MAKE_FLAGS) | tee log/xmake.out

%: fake_tgt
	make  -f Makefile $@ 2>&1 $(MAKE_FLAGS) | tee log/xmake.out

fake_tgt:
	@echo starting make.

.PHONY: 
