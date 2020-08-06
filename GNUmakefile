MAKECMDGOALS=
MAKEFLAGS:= -Rr --warn-undefined-variable

CONF_DEFS:=$(wildcard etc/def/*)
CONF_COPY:=$(patsubst etc/def/%, etc/%, $(CONF_DEFS))
CONF_MISS:=$(filter-out $(wildcard $(CONF_COPY)), $(CONF_COPY))

X:= $(MAKECMDGOALS)

default: Makefile
	@mkdir -p log
	make -f Makefile $(MAKEFLAGS) $(MAKECMDGOALS) | tee log/make.out

Makefile: $(CONF_MISS)
	touch Makefile

$(CONF_MISS): etc/%: etc/def/%
	cp $< $@

$(CONF_MISS): etc/%: etc/def/%
	cp $< $@

GNUmakefile: ;
