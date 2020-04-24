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

class goals_t : public map< sym_t, pct_t >
{
public:
  mapped_type& operator[]( const key_type& k )
  {
    return map< sym_t, pct_t >::operator[]( k );
  };
  mapped_type operator[]( const key_type& k ) const
  {
    auto pos = find( k );
    if ( pos != end() )
      return pos->second;
    else
      return 0;
  };
};

goals_t const& mk_goals()
{
  static goals_t res;
  
  res["BCH"]=98;
  res["BTC"]=98;
  res["DGB"]=98;
  res["HIVE"]=98;
  res["BSV"]=98;
  res["GEO"]=10;

  double tot = 0;
  for ( auto goal : res )
  {
    tot = tot + (double)goal.second;
  };
  for ( auto& goal : res )
  {
    goal.second = (double)goal.second / tot;
  };
  int i=0;
  cout << res.size() << endl;
  for ( auto& goal : res )
  {
    cout << goal.first << " " << goal.second;
    if(++i%5)
      cout << " | ";
    else
      cout << endl;
  };
  cout << endl << endl;
  return res;
};
auto const& goals = mk_goals();

using bittrex::xact_limit;

inline money_t abs( money_t rhs )
{
  return ( rhs < 0 ) ? -rhs : rhs;
};

struct todo_t
{
  sym_t     sym;
  money_t   bal;
  money_t   btc;
  pct_t     pct;
  money_t   usd;
  pct_t     pct_goal;
  money_t   btc_goal;
  money_t   btc_del;
  todo_t &operator+=(const todo_t &rhs)
  {
    if( rhs.sym == sym ) {
      bal+=rhs.bal;
    } else {
      bal=nan("mixed");
    };
    pct += rhs.pct;
    usd += rhs.usd;
    btc += rhs.btc;
    pct_goal += rhs.pct_goal;
    btc_goal += rhs.btc_goal;
    btc_del  += rhs.btc_del;
    return *this;
  };
};
struct todo_more {
  bool operator()( const todo_t &lhs, const todo_t &rhs )
  {
    if((lhs.btc_del) < (rhs.btc_del))
      return true;
    if((rhs.btc_del) < (lhs.btc_del))
      return false;
    return lhs.sym < rhs.sym;
  };
};
struct todo_less {
  bool operator()( const todo_t &lhs, const todo_t &rhs )
  {
    if((lhs.btc_del) < (rhs.btc_del))
      return true;
    if((rhs.btc_del) < (lhs.btc_del))
      return false;
    return lhs.sym < rhs.sym;
  };
};
struct todo_size {
  bool operator()( const todo_t &lhs, const todo_t &rhs )
  {
    if(abs(lhs.btc_del) > abs(rhs.btc_del))
      return true;
    if(abs(rhs.btc_del) > abs(lhs.btc_del))
      return false;
    return lhs.sym < rhs.sym;
  };
};
typedef std::vector<todo_t> todo_v;
static void show_todos( todo_v todo );
typedef vector<string> argv_t;
money_t usd_spot =0;
todo_v make_todos()
{
  money_t tot_btc = 0.0;
  typedef std::map< sym_t, todo_t > todo_m;
  todo_m todo_map;
  for ( auto &g : goals )
  {
    todo_map[ g.first ].sym = g.first;
    todo_map[ g.first ].pct_goal = g.second;
  };
  market_l::load_markets();
  for ( auto &b : balance_l::load_balances() )
  {
    if ( 
        ( goals.find( b.sym ) != goals.end() )
        ||
        ( b.usd > 0.99 )
       )
    {
      auto &todo=todo_map[ b.sym ];
      todo.sym = b.sym;
      todo.btc = b.btc;
      todo.usd = b.btc*usd_spot;
      todo.bal = b.bal;
      tot_btc += b.btc;
    };
  }
  {
    todo_v todos;
    for ( auto &item : todo_map ) {
      auto &sym=item.first;
      auto &todo=item.second;

      todo.pct = todo.btc/tot_btc;
      todo.btc_goal = tot_btc * todo.pct_goal.get();
      todo.btc_del = (todo.btc_goal - todo.btc);
      todos.push_back(todo);
    };
    sort(todos.begin(),todos.end(),todo_more());
    show_todos(todos);
  };
  {
    todo_v todos;
    for ( auto &item : todo_map ) {
      auto &sym=item.first;
      auto &todo=item.second;
      if(abs(todo.btc_del) > 15/usd_spot)
        todos.push_back(todo);
    };
    if(todos.size())
      return todos;
    todo_t &btc_todo=todo_map["BTC"];
    if( abs(btc_todo.btc_del*usd_spot)>10 ) {
      money_t bonus = btc_todo.btc_del/2;
      for ( auto &item : todo_map ) {
        auto &sym=item.first;
        auto &todo=item.second;
        auto combo=bonus+todo.btc_del;
        cout << setw(8) << sym << setw(18) << combo << endl;
        if(abs(todo.btc_del+bonus) > 15/usd_spot)
          todos.push_back(todo);
        for( auto b: todos )
          cout << b << end;
      };
      if(todos.size())
        return todos;
    };
  };
  return todo_v();
};
int xmain( const argv_t &args )
{
  bool loop=false;
  for( auto arg : args ) {
    if( arg == "-y" ) {
      bittrex::fake_buys=false;
      cout << "really gonna do it!" << endl;
    } else if ( arg == "-l" ) {
      loop=true;
      cout << "gonna loop" << endl;
    } else {
      cerr << "bad arg: " << arg << endl;
      exit(1);
    };
  };
  sym_t pivot="BTC";
  int trades=0;
  while(true) {
    time_t now=time(0);
    char buffer[1024];
    strftime(buffer,sizeof(buffer),"%F-%H:%M:%S %Z",gmtime(&now));
    cout << left << "now: " << setw(30) << buffer;
    strftime(buffer,sizeof(buffer),"%F-%H:%M:%S %Z",localtime(&now));
    cout << setw(30) << buffer;
    cout << " trades: " << trades << endl;
    usd_spot = market_t::conv("BTC","USDT");
    auto todos=make_todos();
    if(bittrex::orders_pending()) {
      cout << "orders pending!" << endl;
      sleep(5);
      continue;
    };
    if(todos.size() ) {
      for( auto b(todos.begin()), e(todos.end()); b!=e ; b++ )
      {
        if( b->sym == pivot )
          continue;
        cout
          << (b->btc_del<0 ? "buy  " : "sell ")
          << abs(b->btc_del)
          << " BTC ("
          << abs(b->btc_del)*usd_spot
          << " USDT) "
          << b->sym
          << endl;
        if( abs(b->btc_del) < (5/usd_spot) ) {
          cout << "too small" << endl;
          continue;
        };

        //       if(bittrex::fake_buys)
        //         continue;
        xact_limit(
            pivot,
            b->sym,
            min(b->btc_del*usd_spot,money_t(50)),
            "USDT",
            true
            );
        ++trades;
      };
    };
    if(!loop)
      break;
    sleep(15-(time(0)%15));
    
  };
  return 0;
};
void show_todos( todo_v todo ) {
  if(!todo.size()) {
    cerr << "todo is empty" << endl;
    return;
  };
  auto const& temp = todo[0];
  auto symw=temp.sym.get_width();
  auto monw=temp.btc.get_width();
  auto pctw=temp.pct_goal.get_width();
  cout
    << left
    << "|" << setw( symw ) << "SYM "
    << right
    << "|" << setw( monw ) << "spot "
    << "|" << setw( pctw ) << "cur% "
    << "|" << setw( monw ) << "cur$ "
    << "|" << setw( monw ) << "curB$ "
    << "|" << setw( pctw ) << "goal% "
    << "|" << setw( monw ) << "goal$ "
    << "|" << setw( monw ) << "goalB$ "
    << "|" << setw( monw ) << "delB$"
    << "|" << setw( monw ) << "del$"
    << "|" << setw( pctw ) << "del%"
    << "|" << endl;
  todo.push_back(todo_t());
  todo_t &tot=todo.back();;
  tot.sym = "Total";
  money_t tot_spot;
  for ( auto t : todo ) {
    tot += t;
    money_t spot = market_t::conv(t.sym,"USDT");
    if(t.sym=="Total")
      spot=nan("total");
    cout
      << "|" 
      << left
      << setw(symw) << t.sym << "|"
      << right
      << setw(monw) << spot << "|"
      << setw(pctw) << t.pct << "|"
      << setw(monw) << t.usd << "|"
      << setw(monw) << t.btc << "|"
      << setw(pctw) << t.pct_goal << "|"
      << setw(monw) << t.btc_goal*usd_spot << "|"
      << setw(monw) << t.btc_goal << "|"
      << setw(monw) << t.btc_del << "|"
      << setw(monw) << t.btc_del*usd_spot << "|"
      << setw(pctw) << pct_t(t.btc_del/t.btc) << "|"
      << endl;
  };
};
class fd_streambuf : public streambuf
{
  int fd1;
  int fd2;
  public:
  fd_streambuf(int fd1, int fd2)
    : fd1(fd1)
      , fd2(fd2)
  {
  };
  int overflow(int c = EOF )
  {
    char tmp[1];
    tmp[0]=(c&0xff);
    write(fd1,tmp,1);
    write(fd2,tmp,1);
    return traits_type::to_int_type( c );
  };
  virtual ~fd_streambuf();
};
fd_streambuf::~fd_streambuf()
{
  if(cout.rdbuf()==this)
    cout.rdbuf(0);
  if(cerr.rdbuf()==this)
    cerr.rdbuf(0);
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
    int log=xopen("log/bal.log",O_WRONLY|O_CREAT|O_APPEND,0644);
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
