MAKEFLAGS+= -rR
include etc/default_target.mk
include etc/chain.mk

Makefile: ;

all:=

PWD:=$(shell pwd)

LDLIBS := -Wl,--start-group
LDLIBS += -lcoin
LDLIBS += -lcurl
LDLIBS += -lcurlpp
LDLIBS += -Wl,--end-group

LCOIN_SRC:=$(wildcard lib/src/*.cc)
LCOIN_OBJ:=$(patsubst lib/src/%.cc,lib/obj/%.o,$(LCOIN_SRC))
LCOIN_DEP:=$(patsubst %.o, %.d, $(LCOIN_OBJ))
LCOIN_LIB:=lib/libcoin.a

$(LCOIN_OBJ): etc/chain.mk
#all+= $(LCOIN_LIB) $(LCOIN_OBJ)

ifeq (0,1)
TESTS_SRC:=$(wildcard test/src/*.cc)
TESTS_OBJ:=$(patsubst test/src/%.cc,test/obj/%.o,$(TESTS_SRC))
TESTS_DEP:=$(patsubst %.o, %.d, $(TESTS_OBJ))
TESTS:=    $(patsubst test/src/%.cc,test/bin/%,$(TESTS_SRC))
else
TESTS_SRC:= $(shell echo)
TESTS_OBJ:= $(shell echo)
TESTS_DEP:= $(shell echo)
TESTS:= $(shell echo)
endif
EXES_SRC:=$(wildcard src/*.cc)
EXES_OBJ:=$(patsubst src/%.cc, obj/%.o, $(EXES_SRC))
EXES_DEP:=$(patsubst %.o, %.d, $(EXES_OBJ))
EXES:=$(patsubst src/%.cc, bin/%, $(EXES_SRC))

ALL_DEP:= $(EXES_DEP)
ALL_DEP+= $(LCOIN_DEP)
ALL_DEP+= $(TESTS_DEP)
PRE_DEP:= $(wildcard $(ALL_DEP))
MIS_DEP:= $(filter-out $(PRE_DEP),$(ALL_DEP))

$(EXES_OBJ): etc/chain.mk

all+=$(EXES)
#all+= $(TESTS) $(TESTS_OBJ)

test_%: bin/%
	./$<

ETC_FLAGS:=etc/ar_flags etc/cppflags etc/cxxflags etc/ld_flags
ETC_FLAGS_P:=$(wildcard $(ETC_FLAGS)),$(ETC_FLAGS)
ETC_FLAGS_M:=$(filter-out $(ETC_FLAGS_P),$(ETC_FLAGS))

$(LCOIN_LIB): $(LCOIN_OBJ)
	flock $@.lock $(AR) $(ARFLAGS) $@ $^

$(LCOIN_OBJ): lib/obj/%.o: lib/src/%.cc etc/cppflags etc/cxxflags
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(TESTS_OBJ): test/obj/%.o: test/src/%.cc etc/cppflags etc/cxxflags
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(EXES_OBJ): obj/%.o: src/%.cc etc/cppflags etc/cxxflags
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
	
$(EXES): bin/%: obj/%.o $(LCOIN_LIB) etc/ld_flags
	@mkdir -p $(dir $@)
	$(CXX) $(LDFLAGS) $< -o $@ $(LDLIBS)

$(TESTS): test/bin/%: test/obj/%.o $(LCOIN_LIB)
	$(CXX) $(LDFLAGS) $< -o $@ $(LDLIBS)

all_deps: $(ALL_DEP)
	perl all_deps.pl $@ $^

tags: all_deps
	ctags -L $<

clean:
	rm -f $(TESTS) $(LCOIN_OBJ) $(EXES_OBJ)
	rm -f $(patsubst %.o,%.dd,$(LCOIN_OBJ) $(EXES_OBJ))
	rm -f $(LCOIN_LIB)

all: $(all)
	make tags


