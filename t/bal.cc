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
    auto &res=map< sym_t, pct_t >::operator[]( k );
    if(res.get()>0)
     cerr << "warning: " << k << " set" << endl;
    return res;
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
  

res["BCH"]    =88;  res["BTC"]   =88;  res["BSV"]  =88;  res["XLM"]    =88;  res["LTC"]   =88;
res["USDT"]   =40;

res["DASH"]   =22;   res["ZEC"]   =22;   res["BAT"]  =22;   res["STEEM"]  =22;   res["XRP"]   =22;
res["ETC"]    =22;   res["RVN"]   =22;   res["LBC"]  =22;   res["ZEN"]    =22;   res["ETH"]   =22;

res["RDD"]    =10;   res["SC"]    =10;   res["SLS"]  =10;   res["SPND"]   =10;   res["UKG"]   =10;
res["WAXP"]   =10;   res["XMY"]   =10;   res["XST"]  =10;   res["XTZ"]    =10;   res["ZRX"]   =10;

res["ADA"]    =10;   res["AEON"]  =10;   res["ARK"]  =10;   res["ATOM"]   =10;   res["BTM"]   =10;
res["CURE"]   =10;   res["DAI"]   =10;   res["DGB"]  =10;   res["ENJ"]    =10;   res["EOS"]   =10;
res["GBYTE"]  =10;   res["GEO"]   =10;   res["HBD"]  =10;   res["HIVE"]   =10;   res["KMD"]   =10;
res["LINK"]   =10;   res["NMR"]   =10;   res["NXS"]  =10;   res["PAX"]    =10;   res["PINK"]  =10;


  double tot = 0;
  for ( auto goal : res )
  {
    tot = tot + (double)goal.second;
  };
  cout << "tot: " << tot << endl;
  for ( auto& goal : res )
  {
    goal.second = (double)goal.second / tot;
  };
  cout << res.size() << endl;
  map<double,vector<string>> ranks;
  for ( auto& goal : res )
  {
    ranks[goal.second.get()].push_back(goal.first);
  };
  for ( auto &rank : ranks ) { 
    auto ids=rank.second;
    sort(ids.begin(),ids.end());
    auto itr(ids.begin());
    while(itr!=ids.end()) {
      cout << setw(8) << pct_t(rank.first);
      for( int i = 0; i < 10; i++ )
      {
        if(itr==ids.end())
          break;
        cout << left << setw(10) << *itr++;
      };
      cout << endl;
    };
  };
  cout << endl << endl;
  if(tot!=1000)
    exit(0);
  return res;
};

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
  todo_t(const sym_t &sym=sym_t())
    :sym(sym)
  {
    assert(!bal);
    assert(!pct);
    assert(!btc);
    assert(!usd);
    assert(!pct_goal);
    assert(!btc_goal);
    assert(!btc_del);
  };
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
    if((lhs.btc_del) > (rhs.btc_del))
      return true;
    if((rhs.btc_del) > (lhs.btc_del))
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
typedef vector<string> argv_t;
money_t usd_spot =0;
money_t usd_min_size=15;
money_t min_size() {
  return usd_min_size/usd_spot;
};
struct header_t {
  const todo_t &obj;
  bool dashes;
  header_t(const todo_t &obj, bool dashes)
    : obj(obj), dashes(dashes)
  {
  };
};

ostream &operator<<(ostream &lhs, const header_t &head)
{
  auto const& temp = head.obj;
  auto symw=temp.sym.get_width();
  auto monw=temp.btc.get_width();
  auto pctw=temp.pct_goal.get_width();
  ostringstream text;
  text
    << left
    << "|" << setw( symw ) << "SYM "
    << right
    << "|" << setw( monw ) << "spot$ "
    << "|" << setw( monw ) << "spotB$ "
    << "|" << setw( pctw ) << "cur% "
    << "|" << setw( monw ) << "cur$ "
    << "|" << setw( monw ) << "curB$ "
    << "|" << setw( pctw ) << "goal% "
    << "|" << setw( monw ) << "goal$ "
    << "|" << setw( monw ) << "goalB$ "
    << "|" << setw( monw ) << "delB$"
    << "|" << setw( monw ) << "del$"
    << "|" << setw( pctw ) << "del%"
    << "|";
  string str=text.str();
  if(head.dashes) {
    for(auto &ch : str)
    {
      if(ch=='|')
        ch='+';
      else
        ch='-';
    };
  };
  lhs << str;
  return lhs;
};
inline ostream &operator<<(ostream &lhs, todo_t rhs)
{
  auto symw=rhs.sym.get_width();
  auto monw=rhs.btc.get_width();
  auto pctw=rhs.pct_goal.get_width();
  money_t spot_btc = market_t::conv(rhs.sym,"BTC");
  money_t spot_usd = spot_btc*usd_spot;
  if(rhs.sym=="Total")
    spot_usd=spot_btc=nan("total");
  lhs
    << "|" 
    << left
    << setw(symw) << rhs.sym << "|"
    << right
    << setw(monw) << spot_usd << "|"
    << setw(monw) << spot_btc << "|"
    << setw(pctw) << rhs.pct << "|"
    << setw(monw) << rhs.usd << "|"
    << setw(monw) << rhs.btc << "|"
    << setw(pctw) << rhs.pct_goal << "|"
    << setw(monw) << rhs.btc_goal*usd_spot << "|"
    << setw(monw) << rhs.btc_goal << "|"
    << setw(monw) << rhs.btc_del << "|"
    << setw(monw) << rhs.btc_del*usd_spot << "|"
    << setw(pctw) << pct_t(rhs.btc_del/rhs.btc) << "|"
    ;
  return lhs;
};
todo_v mk_todos()
{
  auto const& goals = mk_goals();
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
    auto &btc=todo_map["BTC"];
    {
      todo_v todos;
      for ( auto &item : todo_map ) {
        auto &sym=item.first;
        auto &todo=item.second;

        todo.pct = todo.btc/tot_btc;
        todo.btc_goal = tot_btc * todo.pct_goal.get();
        todo.btc_del = (todo.btc_goal - todo.btc);
        if(todo.sym != "BTC")
          todos.push_back(todo);
      };
      sort(todos.begin(),todos.end(),todo_less());
      cout << " " << header_t(btc,true) << endl;
      cout << " " << header_t(btc,false) << endl;

      {
        vector<sym_t> avoid;// = { "USDT", "ETH", "LBC" };
        todo_v willdo;
        int i=0;
        todo_t tot_all("Total");
        tot_all+=btc;
        money_t btc_del=btc.btc_del;
        cout << " " << header_t(btc,true) << endl;
        cout << " " << btc << endl;
        while(i<todos.size())
        {
          auto &todo=todos[i++];
          tot_all+=todo;
          cout << " " << todo << endl;
          auto pos=find(avoid.begin(), avoid.end(), todo.sym);
          if(  pos!= avoid.end() )
            continue;
          if( abs(todo.btc_del) < min_size() )
            continue;
          willdo.push_back(todo);
        };
        cout << "%" << tot_all << endl;
        cout << " " << header_t(btc,true) << endl;
        if(willdo.size())
          return willdo;
      };
    }
    return todo_v();
  };
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
    cout << left << " now: " << setw(30) << buffer;
    strftime(buffer,sizeof(buffer),"%F-%H:%M:%S %Z",localtime(&now));
    cout << setw(30) << buffer;
    cout << " trades: " << trades << endl;
    usd_spot = market_t::conv("BTC","USDT");
    auto todos=mk_todos();
    for( auto & todo: todos )
    {
      if( todo.sym == "BTC" )
        continue;
      market_l marks=market_l::get_conv(todo.sym,"BTC");
      if(marks.size() != 1) {
        cout << "cannot do " << todo.sym << " no BTC market";
        return 1;
      };
    };
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
            min(b->btc_del*usd_spot,money_t(100)),
            "USDT",
            true
            );
        ++trades;
      };
      if(!(loop||bittrex::fake_buys)) {
        mk_todos();
      };
    };
    if(!loop)
      break;
    sleep(15-(time(0)%15));
    
  };
  return 0;
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

#define xcroak(x) do{ cerr << x << endl; abort(); }while(0)

int xrename(const char *ofn, const char *nfn)
{
  int res=rename(ofn,nfn);
  if(res<0)
    xcroak("rename("<<ofn<<","<<nfn<<"):" << strerror(errno));
  return res;
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
int open_log(const string &fn)
{
  struct stat stat_buf;
  if(!stat(fn.c_str(),&stat_buf))
  {
    for(int i=100;i<999;i++)
    {
      string bfn=fn+"."+lexical_cast<string>(i);
      if(!stat(bfn.c_str(),&stat_buf))
        continue;
      if(errno!=ENOENT)
        continue;
      xrename(fn.c_str(),bfn.c_str());
      break;
    };
  };
  return xopen(fn.c_str(),O_WRONLY|O_CREAT|O_APPEND,0644);
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
    int log=open_log("log/bal.log");
    assert(log==1);
    xdup2(1,2);
    static fd_streambuf obuf(1,tty);
    static fd_streambuf ibuf(2,tty);
    cout.rdbuf(&obuf);
    cerr.rdbuf(&ibuf);
    cout << "log/bal.log:1:started\n";
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
