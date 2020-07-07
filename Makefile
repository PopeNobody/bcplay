include etc/default_target.mk

all:


#Make
MAKEFLAGS:= -Rr --warn-undefined-variable
MAKEFLAGS:= $(shell touch etc/make_jobs_flag)

AR:= ar

CXXFLAGS=@cxxflags
CPPFLAGS=  -MD -MT $@ @cppflags
LDFLAGS := @ld_flags -L$(PWD)/lib
LDFLAGS += -L$(HOME)/lib
LDFLAGS += -g

LDLIBS := -Wl,--start-group
LDLIBS += -lcoin
LDLIBS += -lcurl
LDLIBS += -lcurlpp
LDLIBS += -Wl,--end-group

LCOIN_SRC:=$(wildcard lib/*.cc)
LCOIN_OBJ:=$(patsubst %.cc,%.o,$(LCOIN_SRC))
LCOIN_MOD:=$(patsubst lib/%.cc,%,$(LCOIN_SRC))

TESTS_SRC:=$(wildcard t/*.cc)
TESTS_OBJ:=$(patsubst %.cc,%.o,$(TESTS_SRC))
TESTS_MOD:=$(patsubst t/%.cc,%,$(TESTS_SRC))
TESTS:=$(TESTS_MOD)

test_%: %
	report ./$<



test_markets: markets
	./$< BCH BSV

ARFLAGS:= Urv

LCOIN_OBJ: $(LCOIN_OBJ)
	echo "LCOIN_OBJ: done"

libcoin.a: $(LCOIN_OBJ)
	flock $@.lock $(AR) $(ARFLAGS) $@ $^

%.o: %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -E $< -o $(<:.cc=.ii)
	$(CXX) $(CXXFLAGS) -c $(<:.cc=.ii) -o $@

%.i:
	make $(@:.i:.o)

$(TESTS): %: t/%.o libcoin.a
	$(CXX) $(LDFLAGS) $< -o $@ $(LDLIBS)

test: $(TESTS)

all: tags $(TESTS)


CTAGS_FLAGS:= --extra=fq --fields=afikKlmnsSzt --language-force=c++

clean:
	rm -f libcoin.a $(TESTS)
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

.PHONY: sums

sums:
	git ls-files | xargs md5sum | tee sums

include $(wildcard /dev/null */*.d)
#-L/usr/lib/x86_64-linux-gnu/mit-krb5 -lcurl -lnghttp2 -lidn2 -lrtmp -lpsl -lssl -lcrypto -lssl -lcrypto -Wl,-Bsymbolic-functions -Wl,-z,relro -lgssapi_krb5 -lkrb5 -lk5crypto -lcom_err -llber -lldap -llber -lz
