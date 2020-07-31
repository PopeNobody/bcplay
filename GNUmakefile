
MAKE_FLAGS:= -Rr --warn-undefined-variable $(shell cat etc/make_jobs_flag)

CONF_DEFS:=$(wildcard etc/def/*)
CONF_COPY:=$(patsubst etc/def/%, etc/%, $(CONF_DEFS))
CONF_MISS:=$(filter-out $(wildcard $(CONF_COPY)), $(CONF_COPY))


default: Makefile
	@mkdir -p log
	${MAKE} -f Makefile $(MAKE_FLAGS) $(MAKE_CMD_GOALS) | tee log/make.out

Makefile: $(CONF_MISS)
	touch Makefile

$(CONF_MISS): etc/%: etc/def/%
	cp $< $@

$(CONF_MISS): etc/%: etc/def/%
	cp $< $@

GNUmakefile: ;
