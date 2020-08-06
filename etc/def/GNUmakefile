MAKE_FLAGS:= -Rr --warn-undefined-variable
MAKEFLAGS:= -Rr

MAKECMDGOALS:=$(if $(MAKECMDGOALS),$(MAKECMDGOALS),)

CONF_DEFS:=$(wildcard etc/def/*)
CONF_COPY:=$(patsubst etc/def/%, etc/%, $(CONF_DEFS))
CONF_MISS:=$(filter-out $(wildcard $(CONF_COPY)), $(CONF_COPY))

$(warning $(CONF_MISS))

X:= $(MAKECMDGOALS)

default: Makefile
	@mkdir -p log
	make  -f Makefile $(MAKE_FLAGS) $(MAKECMDGOALS) | tee log/make.out

$(MAKECMDGOALS): Makefile
	@mkdir -p log
	make  -f Makefile $(MAKE_FLAGS) $(MAKECMDGOALS) | tee log/make.out

Makefile: $(CONF_MISS)
	touch Makefile

$(CONF_MISS): etc/%: etc/def/%
	cp $< $@

$(CONF_MISS): etc/%: etc/def/%
	cp $< $@

GNUmakefile: ;

