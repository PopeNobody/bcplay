MAKEFLAGS:= -Rr
include etc/default_target.mk

Makefile: ;

all:=

CXX:=g++
CXXFLAGS=@etc/cxxflags

CPPFLAGS=  -MD -MT $@ @etc/cppflags

AR:= ar
ARFLAGS:= Urv

LDFLAGS := @etc/ld_flags -L$(PWD)/lib
LDFLAGS += -L$(HOME)/lib
LDFLAGS += -g

LDLIBS := -Wl,--start-group
LDLIBS += -lcoin
LDLIBS += -lcurl
LDLIBS += -lcurlpp
LDLIBS += -Wl,--end-group

LCOIN_SRC:=$(wildcard lib/*.cc)
LCOIN_OBJ:=$(patsubst lib/%.cc,lib/%.oo,$(LCOIN_SRC))
LCOIN_LIB:=lib/libcoin.a

all+= $(LCOIN_LIB) $(LCOIN_OBJ)

TESTS_SRC:=$(wildcard test/src/*.cc)
TESTS_OBJ:=$(patsubst test/src/%.cc,test/obj/%.oo,$(TESTS_SRC))
TESTS:=    $(patsubst test/src/%.cc,test/bin/%,$(TESTS_SRC))

all+= $(TESTS) $(TESTS_OBJ)

ETC_FLAGS:=etc/ar_flags etc/cppflags etc/cxxflags etc/ld_flags
ETC_FLAGS_P:=$(wildcard $(ETC_FLAGS)),$(ETC_FLAGS)
ETC_FLAGS_M:=$(filter-out $(ETC_FLAGS_P),$(ETC_FLAGS))

$(LCOIN_LIB): $(LCOIN_OBJ)
	flock $@.lock $(AR) $(ARFLAGS) $@ $^

test/obj/%.oo: test/src/%.cc
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -E $< -o $(<:.cc=.ii)
	$(CXX) $(CXXFLAGS) -c $(<:.cc=.ii) -o $@

$(TESTS): test/bin/%: test/obj/%.oo $(LCOIN_LIB)
	$(CXX) $(LDFLAGS) $< -o $@ $(LDLIBS)

CLEAN_OBJ:= $(patsubst %.oo, %.ii %.oo, $(LCOIN_OBJ) $(TESTS_OBJ))
CLEAN_TAR:= $(TESTS) $(LCOIN_LIB)

clean:
	rm -f $(TESTS) $(LCOIN_LIB) $(CLEAN_OBJ) $(CLEAN_TAR) tag

all: $(all)
