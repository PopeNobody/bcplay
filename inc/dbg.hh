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
#define trace(x,y)  macwrap( \
    dbg::show_dbg_msg(__FILE__,__LINE__) << x << endl, \
    y)
#define  expose(x)    trace(  #x << " => " <<  x,  nop()  )
#define  carp(x)     trace(  "warning:             "      <<  x,  nop()        )
#define  croak(x)    trace(  "error:               "      <<  x,  abort()      )
#define  confess(x)  trace(  "confess:             "      <<  x,  dump(true)   )
#define  cluck(x)    trace(  "cluck:               "      <<  x,  dump(false)  )
#define  checkin()   trace(  __PRETTY_FUNCTION__,  nop()  )

#endif //debug_hh
