test: all
	./bal

#Make
MAKEFLAGS:= -Rr --warn-undefined-variable -j6

#CXX
CXX:= clang++
CXXFLAGS += -g
CXXFLAGS += -fPIC
CXXFLAGS += -DWITH_ICU -I$(HOME)/opt/include
#AR
AR:= ar

#CPP
CPPFLAGS :=
CPPFLAGS += -I inc -MD

#LD
LDFLAGS += -L$(HOME)/opt/lib
LDFLAGS += -g -L. 

LDLIBS := 
LDLIBS += -lcoin
LDLIBS += -lcurl -lcurlpp
LDLIBS += -lbitcoin 
LDLIBS += -lsecp256k1 -lgmp
LDLIBS += -lboost_system
LDLIBS += -lboost_thread
LDLIBS += -lboost_regex
LDLIBS += -lboost_locale
LDLIBS += -lboost_program_options
LDLIBS += -licudata
LDLIBS += -ldl

LCOIN_SRC:=$(wildcard lib/*.cc)
LCOIN_OBJ:=$(patsubst %.cc,%.o,$(LCOIN_SRC))
LCOIN_MOD:=$(patsubst lib/%.cc,%,$(LCOIN_SRC))
LCOIN:=$(MYLIB_MOD)

TESTS_SRC:=$(wildcard t/*.cc)
TESTS_OBJ:=$(patsubst %.cc,%.o,$(TESTS_SRC))
TESTS_MOD:=$(patsubst t/%.cc,%,$(TESTS_SRC))
TESTS:=$(TESTS_MOD)

ARFLAGS:= Urv

LCOIN_OBJ: $(LCOIN_OBJ)
	echo "LCOIN_OBJ: done"

libcoin.a: $(LCOIN_OBJ)
	flock $@.lock $(AR) $(ARFLAGS) $@ $^

%.o: %.ii
	$(CXX) $(CXXFLAGS) -E $< -o $(<:.cc=.ii)

%.ii: %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -E $< -o $(<:.cc=.ii)

$(TESTS): %: t/%.o libcoin.a
	$(CXX) $(LDFLAGS) $< -o $@ $(LDLIBS) -lpthread

test: $(TESTS)

all: $(TESTS)


CTAGS_FLAGS:= --extra=fq --fields=afikKlmnsSzt

clean:
	rm -f libcoin.a $(TESTS)
	rm -f tags deps.all
	rm -f */*.[od] */*.ii

tags:	deps.all
	ctags $(CTAGS_FLAGS) -L $^
#    
deps.all: $(wildcard $(patsubst %.cc,%.d,$(wildcard */*.cc)))
	all_deps $^ -o $@

include $(wildcard /dev/null */*.d)
#-L/usr/lib/x86_64-linux-gnu/mit-krb5 -lcurl -lnghttp2 -lidn2 -lrtmp -lpsl -lssl -lcrypto -lssl -lcrypto -Wl,-Bsymbolic-functions -Wl,-z,relro -lgssapi_krb5 -lkrb5 -lk5crypto -lcom_err -llber -lldap -llber -lz
