include etc/default_target.mk

all:
MAKEFLAGS:= -pd -Rr --warn-undefined-variable 

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

TESTS_SRC:=$(wildcard test/src/*.cc)
TESTS_OBJ:=$(patsubst test/src/%.cc,test/obj/%.oo,$(TESTS_SRC))
TESTS:=    $(patsubst test/src/%.cc,test/bin/%,$(TESTS_SRC))

ETC_FLAGS:=etc/ar_flags etc/cppflags etc/cxxflags etc/ld_flags
ETC_FLAGS_P:=$(wildcard $(ETC_FLAGS)),$(ETC_FLAGS)
ETC_FLAGS_M:=$(filter-out $(ETC_FLAGS_P),$(ETC_FLAGS))

etc $(ETC_FLAGS_M): %: %.def
	cp $< $@



test_%: %
	report ./$<



test_markets: markets
	./$< BCH BSV

ARFLAGS:= Urv

LCOIN_OBJ: $(LCOIN_OBJ)
	echo "LCOIN_OBJ: done"

lib/libcoin.a: $(LCOIN_OBJ)
	flock $@.lock $(AR) $(ARFLAGS) $@ $^

test/obj/%.oo: test/src/%.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -E $< -o $(<:.cc=.ii)
	$(CXX) $(CXXFLAGS) -c $(<:.cc=.ii) -o $@

$(TESTS): test/bin/%: test/src/%.oo lib/libcoin.a
	$(CXX) $(LDFLAGS) $< -o $@ $(LDLIBS)

test: $(TESTS)

all: $(TESTS)


CTAGS_FLAGS:= --extra=fq --fields=afikKlmnsSzt --language-force=c++

clean:
	rm -f lib/libcoin.a $(TESTS)
	rm -f tags deps.all
	rm -f */*.[od] */*.ii
	LC_ALL=C LANG=C sort .gitignore -o .gitignore -u

tags:	deps.all
	ctags $(CTAGS_FLAGS) -L $^

DEPS:= /dev/null $(wildcard $(patsubst %.cc,%.d,$(wildcard */*.cc)))
$(DEPS): ;
deps.all: $(DEPS)
	vi_perl all_deps.pl  $@ $^
	wc -l $@

.PHONY: sums

sums:
	git ls-files | xargs md5sum | tee sums

include $(wildcard /dev/null */*.d)
#-L/usr/lib/x86_64-linux-gnu/mit-krb5 -lcurl -lnghttp2 -lidn2 -lrtmp -lpsl -lssl -lcrypto -lssl -lcrypto -Wl,-Bsymbolic-functions -Wl,-z,relro -lgssapi_krb5 -lkrb5 -lk5crypto -lcom_err -llber -lldap -llber -lz
