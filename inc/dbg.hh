#define NDEBUG
#include <assert.h>

#ifndef debug_hh
#define debug_hh debug_hh

#include <iostream>

namespace dbg 
{
  using std::ostream;
  ostream &show_dbg_msg(const char *file, unsigned line, const char *msg=0 );
}
#define nop()
#define macwrap(x,y)  do{ x; y; }while(0)
#define xtrace2(x,y)  macwrap( \
    dbg::show_dbg_msg(__FILE__,__LINE__) << x << endl, \
    y)
#define xtrace(x) xtrace2(x,nop())
#define  xexpose(x)    xtrace2(  #x << " => " <<  x,  nop()  )
#define  xcarp(x)     xtrace2(  "warning:             "      <<  x,  nop()        )
#define  xcroak(x)    xtrace2(  "error:               "      <<  x,  abort()      )
#define  xconfess(x)  xtrace2(  "confess:             "      <<  x,  dump(true)   )
#define  xcluck(x)    xtrace2(  "cluck:               "      <<  x,  dump(false)  )
#define  xcheckin()   xtrace2(  __PRETTY_FUNCTION__,  nop()  )
#define  xassert(x)   if(!(x)) { xthrowre( "assertion failed: '" << #x ); }
#define  xnv(x) #x << " => " << (x)
#define dbg() __FILE__ << ":" << __LINE__ << ":"
#define  xthrow(x,y) do{ \
  stringstream msg; \
  msg << "xthrow:" << endl << dbg() << y << endl; \
  throw x(msg.str()); \
}while(false)
#define  xthrowre(y) xthrow(runtime_error,y)

#endif //debug_hh
