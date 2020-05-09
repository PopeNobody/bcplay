#include <bittrex_json.hh>
#include <fmt.hh>
#include <json.hh>
#include <typeinfo>
#include <util.hh>
#include <web_api.hh>
#include <fcntl.h>
#include <util.hh>

using namespace util;
using namespace coin;
using namespace fmt;

using namespace std;

using std::ostream;
using std::exception;
using std::type_info;
ostream &operator<<(ostream &lhs, const exception &rhs)
{
  return lhs<<rhs.what();
};
ostream &operator<<(ostream &lhs, const type_info &rhs)
{
  return lhs<<demangle(typeid(rhs).name());
};

typedef map<sym_t,double> goals_t;
goals_t goals;

typedef vector<string> argv_t;
money_t usd_spot =0;
int xmain( const argv_t &args )
{
  for( auto arg : args ) {
    if( arg == "-y" ) {
      bittrex::fake_buys=false;
      cout << "really gonna do it!" << endl;
    } else {
      cerr << "bad arg: " << arg << endl;
      exit(1);
    };
  };
  auto config=json::parse(read_file("etc/guard.json"));
  cout << setw(4) << config << endl;

  return 0;
};

string now() {
  time_t time=::time(0);
  char buf[1024];
  struct tm tm;
  tm=*gmtime(&time);
  strftime(buf,sizeof(buf)-1,"%Y-%m-%d %H:%M:%S %Z",&tm);
  return buf;
};
int main( int argc, char** argv )
{
  try
  {
    int tty=xdup(1);
    xclose(0);
    xopen("/dev/null",O_RDONLY);
    mkdir("log",0755);
    int log=xopen("log/guard.log",O_WRONLY|O_APPEND|O_CREAT);
    time_t when=time(0);
    xtrace("starting: " << now());
    xdup2(log,1);
    assert(log==1);
    xdup2(1,2);
    static fd_streambuf obuf(1,tty);
    static fd_streambuf ibuf(2,tty);
    cout.rdbuf(&obuf);
    cerr.rdbuf(&ibuf);
    cout << "log/guard.log:1:started\n";
    argv_t args( argv+1, argv+argc );
    if ( xmain( args ) )
      return 1;
    return 0;
  }
  catch ( const exception& e )
  {
    xexpose(e.what());
  }
  catch ( ... )
  {
    xtrace("wtf?");
  }
  return 1;
};
