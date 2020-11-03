CXX:=clang++-11
CXXFLAGS = @etc/cxxflags
CPPFLAGS += -MD -MT $@ @etc/cppflags
LDFLAGS := 
LDFLAGS += -L/home/nn/src/bc/lib64
LDFLAGS += @etc/ld_flags -L$(PWD)/lib
AR:= ar
ARFLAGS:= Urv
