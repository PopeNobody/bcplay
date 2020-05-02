#include <bittrex.hh>
#include <fmt.hh>
#include <typeinfo>
#include <util.hh>
#include <web_api.hh>
#include <fcntl.h>

using namespace util;
using namespace coin;
using namespace fmt;
using namespace std;

typedef vector<string> argv_t;
int xmain( const argv_t &args )
{
  return 0;
};
int xdup2(int fd, int ofd)
{
  int res=dup2(fd,ofd);
  if(res<0)
    assert(!"dup2 failed");
  return res;
};
int xdup(int fd)
{
  int res=dup(fd);
  if(res<0)
    assert(!"dup failed");
  return res;
};
int xopen(const char *fn, int flags, int mode=0644)
{
  int res=open(fn,flags,mode);
  if(res<0)
    assert(!"open failed");
  return res;
};
int xclose(int fd)
{
  auto res=close(fd);
  if(res<0)
    assert(!"close failed");
  return res;
};
int main( int argc, char** argv )
{
  try
  {
    int tty=xdup(1);
    xclose(0);
    xopen("/dev/null",O_RDONLY);
    xclose(1);
    mkdir("log",0755);
    int log=xopen("log/info.log",O_WRONLY|O_CREAT|O_APPEND,0644);
    assert(log==1);
    xdup2(1,2);
    static fd_streambuf obuf(1,tty);
    static fd_streambuf ibuf(2,tty);
    cout.rdbuf(&obuf);
    cerr.rdbuf(&ibuf);
    argv_t args( argv+1, argv+argc );
    if ( xmain( args ) )
      return 1;
    return 0;
  }
  catch ( exception& e )
  {
    cout << nl << nl << e.what() << nl << nl;
  }
  catch ( ... )
  {
    cout << nl;
    cout << nl;
    cout << nl;
    cout << "wtf?" << nl;
  }
  return 1;
};
