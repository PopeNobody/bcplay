#    test: test_prices
#    

test_get_order_hist: all

test_bal: all

test_ftest: all

test_ppjson: all
	./ppjson < balance.json

all:
#Make
MAKEFLAGS:= -Rr --warn-undefined-variable -j6

#CXX
CXX:= clang++
CXXFLAGS += -g
CXXFLAGS += -fPIC
#AR
AR:= ar

#CPP
CPPFLAGS :=
CPPFLAGS += -I inc
CPPFLAGS += -DWITH_ICU -I$(HOME)/include
CPPFLAGS += -DSYSCONFDIR="\"/home/rfp/stow/bx/etc\""

#LD
LDFLAGS += -L$(HOME)/lib
LDFLAGS += -g -L. 

LDLIBS := -Wl,--start-group
LDLIBS += -lcoin
LDLIBS += -lcurl -lcurlpp
LDLIBS += -lbitcoin-system
LDLIBS += -lsecp256k1 -lgmp
LDLIBS += -lboost_system
LDLIBS += -lboost_thread
LDLIBS += -lboost_regex
LDLIBS += -lboost_locale
LDLIBS += -lboost_program_options
LDLIBS += -licudata
LDLIBS += -licuuc
LDLIBS += -licui18n
LDLIBS += -ldl
LDLIBS += -lpthread
LDLIBS += -Wl,--end-group

LCOIN_SRC:=$(wildcard lib/*.cc)
LCOIN_OBJ:=$(patsubst %.cc,%.o,$(LCOIN_SRC))
LCOIN_MOD:=$(patsubst lib/%.cc,%,$(LCOIN_SRC))
LCOIN:=$(MYLIB_MOD)

TESTS_SRC:=$(wildcard t/*.cc)
TESTS_OBJ:=$(patsubst %.cc,%.o,$(TESTS_SRC))
TESTS_MOD:=$(patsubst t/%.cc,%,$(TESTS_SRC))
TESTS:=$(filter-out markets,$(TESTS_MOD))

test_%: %
	./$<

test_markets: markets
	./$< BCH BSV

ARFLAGS:= Urv

LCOIN_OBJ: $(LCOIN_OBJ)
	echo "LCOIN_OBJ: done"

libcoin.a: $(LCOIN_OBJ)
	flock $@.lock $(AR) $(ARFLAGS) $@ $^

%.o: %.cc
	$(CXX) $(CPPFLAGS) -MD -MT $@ -MF $(<:.cc=.d) $(CXXFLAGS) -E $< -o $(<:.cc=.ii)
	$(CXX) $(CXXFLAGS) -c $(<:.cc=.ii) -o $@

$(TESTS): %: t/%.o libcoin.a
	$(CXX) $(LDFLAGS) $< -o $@ $(LDLIBS)

test: $(TESTS)

all: $(TESTS)


CTAGS_FLAGS:= --extra=fq --fields=afikKlmnsSzt

clean:
	rm -f libcoin.a{,.lock} $(TESTS)
	rm -f tags deps.all
	rm -f */*.[od] */*.ii

tags:	deps.all
	ctags $(CTAGS_FLAGS) -L $^
#    
DEPS:=$(wildcard $(patsubst %.cc,%.d,$(wildcard */*.cc)))
$(DEPS): ;
deps.all: $(DEPS)
	rm -f $@ tags
	vi_perl all_deps.pl  $@ $^

.PHONY: deps.all

include $(wildcard /dev/null */*.d)
#-L/usr/lib/x86_64-linux-gnu/mit-krb5 -lcurl -lnghttp2 -lidn2 -lrtmp -lpsl -lssl -lcrypto -lssl -lcrypto -Wl,-Bsymbolic-functions -Wl,-z,relro -lgssapi_krb5 -lkrb5 -lk5crypto -lcom_err -llber -lldap -llber -lz
