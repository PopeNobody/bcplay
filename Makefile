MAKEFLAGS:= -Rr
include etc/default_target.mk

Makefile: ;

CXX:=g++
#Make

AR:= ar

CXXFLAGS=@etc/cxxflags
CPPFLAGS=  -MD -MT $@ @etc/cppflags
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

TESTS_SRC:=$(wildcard test/src/*.cc)
TESTS_OBJ:=$(patsubst test/src/%.cc,test/obj/%.oo,$(TESTS_SRC))
TESTS:=    $(patsubst test/src/%.cc,test/bin/%,$(TESTS_SRC))

all: $(TESTS)

ETC_FLAGS:=etc/ar_flags etc/cppflags etc/cxxflags etc/ld_flags
ETC_FLAGS_P:=$(wildcard $(ETC_FLAGS)),$(ETC_FLAGS)
ETC_FLAGS_M:=$(filter-out $(ETC_FLAGS_P),$(ETC_FLAGS))

$(LCOIN_LIB): $(LCOIN_OBJ)
	flock $@.lock $(AR) $(ARFLAGS) $@ $^

test/obj/%.oo: test/src/%.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -E $< -o $(<:.cc=.ii)
	$(CXX) $(CXXFLAGS) -c $(<:.cc=.ii) -o $@

$(TESTS): test/bin/%: test/src/%.oo $(LCOIN_LIB)
	$(CXX) $(LDFLAGS) $< -o $@ $(LDLIBS)

