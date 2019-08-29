test_ckey: all
	./ckey

all:
CXX := clang++-6.0
CXXFLAGS += -std=gnu++2a -g -pthread -I inc -MD
CXXFLAGS += -fPIC
CXXFLAGS += $(DBINCS)
LDFLAGS += -g -L. 

BCLIBS +=  -lbitcoin -lsecp256k1
LDLIBS = -lcurlpp -lcurl -lcoin -lboost_system $(BCLIBS)
DBLIBS = $(shell mariadb_config --libs)
DBINCS = $(shell mariadb_config --include )

DB_MODS:=get_historical mysql_test


LCOIN_SRC:=$(wildcard lib/*.cc)
LCOIN_OBJ:=$(patsubst %.cc,%.o,$(LCOIN_SRC))
LCOIN_MOD:=$(patsubst lib/%.cc,%,$(LCOIN_SRC))
LCOIN_MEM:=$(patsubst %,libcoin.a(%),$(LCOIN_OBJ))
LCOIN:=$(MYLIB_MOD)

TESTS_SRC:=$(wildcard t/*.cc)
TESTS_OBJ:=$(patsubst %.cc,%.o,$(TESTS_SRC))
TESTS_MOD:=$(patsubst t/%.cc,%,$(TESTS_SRC))
TESTS:=$(TESTS_MOD)

ARFLAGS:= Urv

libcoin.a: $(LCOIN_MEM)

%.o: %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(TESTS): %: t/%.o libcoin.a
	$(CXX) $(LDFLAGS) $< -o $@ $(LDLIBS) -lpthread

test: all
	./prices

xxx:
	./ham | tr -d ' \n' | md5sum
	./ham

lcoin_obj: $(LCOIN_OBJ)
tests_obj: $(TESTS_OBJ)
tests: $(TESTS)

all: libcoin.a $(TESTS)


CTAGS_FLAGS:= --extra=fq --fields=afikKlmnsSzt

clean:
	rm -f libcoin.a $(TESTS)
	rm -f tags deps.all
	rm -f */*.[od]

tags:	deps.all
	ctags $(CTAGS_FLAGS) -L $^
#    
deps.all: $(wildcard $(patsubst %.cc,%.d,$(wildcard */*.cc)))
	all_deps $^ -o $@

include $(wildcard /dev/null */*.d)
