MAKEFLAGS+= -rR
include etc/default_target.mk
include etc/chain.mk

Makefile: ;

all:=

PWD:=$(shell pwd)

LCOIN_SRC:=$(wildcard lib/src/*.cc)
LCOIN_OBJ:=$(patsubst lib/src/%.cc,lib/obj/%.o,$(LCOIN_SRC))
LCOIN_LIB:=lib/libcoin.a

#all+= $(LCOIN_LIB) $(LCOIN_OBJ)

ifeq (0,1)
TESTS_SRC:=$(wildcard test/src/*.cc)
TESTS_OBJ:=$(patsubst test/src/%.cc,test/obj/%.o,$(TESTS_SRC))
TESTS:=    $(patsubst test/src/%.cc,test/bin/%,$(TESTS_SRC))
else
TESTS_SRC:= $(shell echo)
TESTS_OBJ:= $(shell echo)
TESTS:= $(shell echo)
endif
EXES_SRC:=$(wildcard src/*.cc)
EXES_OBJ:=$(patsubst src/%.cc, obj/%.o, $(EXES_SRC))
EXES:=$(patsubst src/%.cc, bin/%, $(EXES_SRC))

all+=$(EXES)
#all+= $(TESTS) $(TESTS_OBJ)

test_%: bin/%
	./$<

ETC_FLAGS:=etc/ar_flags etc/cppflags etc/cxxflags etc/ld_flags
ETC_FLAGS_P:=$(wildcard $(ETC_FLAGS)),$(ETC_FLAGS)
ETC_FLAGS_M:=$(filter-out $(ETC_FLAGS_P),$(ETC_FLAGS))

$(LCOIN_LIB): $(LCOIN_OBJ)
	flock $@.lock $(AR) $(ARFLAGS) $@ $^

$(LCOIN_OBJ): lib/obj/%.o: lib/src/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(TESTS_OBJ): test/obj/%.o: test/src/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(EXES_OBJ): obj/%.o: src/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
	
$(EXES): bin/%: obj/%.o $(LCOIN_LIB)
	$(CXX) $(LDFLAGS) $< -o $@ $(LDLIBS)

$(TESTS): test/bin/%: test/obj/%.o $(LCOIN_LIB)
	$(CXX) $(LDFLAGS) $< -o $@ $(LDLIBS)

clean:
	rm -f $(TESTS) $(LCOIN_OBJ) $(EXES_OBJ)
	rm -f $(patsubst %.o,%.dd,$(LCOIN_OBJ) $(EXES_OBJ))
	rm -f $(LCOIN_LIB)

all: $(all)
