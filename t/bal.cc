#include <bittrex.hh>
#include <fmt.hh>
#include <typeinfo>
#include <util.hh>
#include <web_api.hh>
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


using bittrex::xact_limit;

money_t tot_usd=0;
struct todo_t : public fmt::can_str
{
  sym_t sym;
  money_t usd_bal;
  pct_t pct_goal;
  money_t usd_goal;
  money_t usd_delta() const
  {
    return -usd_bal+usd_goal;
  };
  int delta_sign() const
  {
    return usd_delta()/abs(usd_delta());
  };
  ostream &header(ostream &xlhs, int ind=0) const
  {
    ostringstream lhs;
    lhs
      << left << setw(ind) << ""
      << setw(sym.get_width()) << "SYM" << right
      << " " << setw(usd_bal.get_width()-2) << "BAL"<< "__"
      << " " << setw(pct_goal.get_width()-2) << "BAL%"<< "__"
      << " " << setw(pct_goal.get_width()-2) << "G%"<< "__"
      << " " << setw(usd_bal.get_width()-2) << "G$"<< "__"
      << " " << setw(usd_bal.get_width()-2) << "DEL$"<< "__"
      ;
    return xlhs << lhs.str();
  };
  ostream &stream(ostream &lhs, int ind=0) const
  {
    lhs
      << setw(ind) << ""
      << sym
      << " " << usd_bal
      << " ";
    if(tot_usd) {
      lhs << pct_t(usd_bal/tot_usd);
    } else {
      lhs << string(pct_goal.get_width(),'+');
    };
    lhs
      << " " << pct_goal
      << " " << usd_goal
      << " " << usd_delta();
    return lhs;
  };
};
struct todo_abs {
  bool operator()( const todo_t &lhs, const todo_t &rhs )
  {
    if(abs(lhs.usd_delta()) > abs(rhs.usd_delta()))
      return true;
    if(abs(rhs.usd_delta()) < abs(lhs.usd_delta()))
      return false;
    return lhs.sym < rhs.sym;
  };
};
struct todo_less {
  bool operator()( const todo_t &lhs, const todo_t &rhs )
  {
    if(lhs.usd_delta() < rhs.usd_delta())
      return true;
    if(rhs.usd_delta() > lhs.usd_delta())
      return false;
    return lhs.sym < rhs.sym;
  };
};
typedef std::vector<todo_t> todo_v;
ostream &operator<<(ostream &lhs, const todo_v &rhs)
{
  for( auto todo : rhs )
  {
    lhs << "  " << todo << endl;
  };
  return lhs;
};
typedef std::map< sym_t, todo_t > todo_m;
typedef vector<string> arg_v;
goals_t const& mk_goals()
{
  static goals_t res;
  res["BCH"]=200;
  res["BSV"]=200;
  res["BTC"]=100;
  res["DASH"]=100;
  res["RVN"]=100;
  res["XLM"]=100;
  res["XMR"]=95;
  res["XRP"]=5;
  res["ZEN"]=100;

  pct_t tot = 0;
  for( auto g : res ) {
    tot = tot.get()+g.second.get();
  };
  res["USDT"]=tot.get();
  tot = tot.get()+res["USDT"].get();
  for( auto & g : res ) {
    g.second = pct_t(g.second.get()/tot.get());
  };
  return res;
};

static const todo_v mk_todo( )
{
  todo_m todo_sym;
  auto const &balances = balance_l::load_balances();
  auto const &goals = mk_goals();
  {
    multimap<pct_t, sym_t> rgoals;
    for ( auto const & goal : goals )
      rgoals.insert(make_pair(goal.second,goal.first));
    pct_t last;
    for ( auto const & rgoal : rgoals )
    {
      if(last==rgoal.first) {
        cout << rgoal.second;
      } else {
        last=rgoal.first;
        cout << endl << rgoal.first << rgoal.second;
      };
    };
    cout << endl;
  };
  tot_usd=0;
  for ( auto const &b : balances ) {
    tot_usd += b.usd;
    if( b.pend ) {
      cout << b.sym << " has pending balance " << b.pend << endl;
    };
  };
  cout << "tot_usd: " << tot_usd << endl;
  int i=1;
  for ( auto const &g : goals ) 
  {
    auto &todo = todo_sym[g.first];
    todo.sym=g.first;
    auto ptr = balances.get_ptr(g.first);
    if(ptr)
      todo.usd_bal=ptr->usd;
    todo.pct_goal=g.second;
    todo.usd_goal=tot_usd*g.second.get();
  };
  for ( auto const &b : balances )
  {
    if(b.usd < 0.01)
      continue;
    if(todo_sym.find(b.sym)!=todo_sym.end())
      continue;
    auto &todo = todo_sym[b.sym];
    todo.sym=b.sym;
    todo.usd_bal=0;
    todo.pct_goal=0;
    todo.usd_goal=0;
  };
  todo_v todos;
  for ( auto const &todo : todo_sym )
    todos.push_back(todo.second);
  return todos;
};

void todo_sort_show(const string &name, todo_v &todos)
{
  if(name=="ign")
    sort(todos.begin(),todos.end(),todo_less());
  else
    sort(todos.begin(),todos.end(),todo_abs());
  cout << name << ":" << endl;
  if(todos.size())
    cout << todos << endl;
};
money_t min_size;
void split_and_show(const todo_v &todos, todo_v &pos, todo_v &neg, todo_v &ign)
{
  cout << "min_size=" << min_size << endl;
  for( auto todo : todos )
  {
    if( abs(todo.usd_delta()) < min_size ) {
      ign.push_back(todo);
    } else if ( todo.usd_delta() < 0 ) {
      neg.push_back(todo);
    } else if ( todo.usd_delta() > 0 ) {
      pos.push_back(todo);
    };
  };
  cout << endl;
  todo_t().header(cout,2) << endl;
  todo_sort_show("pos",pos);
  todo_sort_show("neg",neg);
  todo_sort_show("ign",ign);
};
auto find_match( const todo_v &todos )
{
  min_size = max(tot_usd*0.0001,money_t(3));
  todo_v pos, neg, ign;
  split_and_show(todos,pos,neg,ign);

  sym_t f_sym, t_sym;
  money_t qty(0);
  for(auto pbeg(pos.begin()), pend(pos.end());pbeg!=pend;pbeg++)
  {
    cout << pbeg->sym;
    for( auto nbeg(neg.begin()), nend(neg.end());nbeg!=nend;nbeg++)
    {
      cout << " " << nbeg->sym;
      auto tmp=min(abs(pbeg->usd_delta()),abs(nbeg->usd_delta()));
      auto marks = market_l::get_conv(pbeg->sym,nbeg->sym);
      if(marks.size()==1){
        cout << "*" << endl;
        cout << *pbeg << endl;
        cout << *nbeg << endl;
        f_sym=pbeg->sym;
        t_sym=nbeg->sym;
        qty=tmp;
        goto out;
      };
    };
    cout << endl;
  };
out:
  if(qty) {
  } else {
    f_sym=t_sym="BTC";
    if ( pos.size() ) {
      if( neg.size() && abs(neg[0].usd_delta())>abs(pos[0].usd_delta()) ) {
        f_sym=neg[0].sym;
        qty=neg[0].usd_delta();
      } else {
        f_sym=pos[0].sym;
        qty=pos[0].usd_delta();
      };
    } else if ( neg.size() ) {
      t_sym=neg[0].sym;
      qty=neg[0].usd_delta();
    };
  };
  if( f_sym == "BTC" && t_sym == "BTC" )
   t_sym="USDT";
  return make_tuple( f_sym, t_sym, qty, f_sym+"-"+t_sym );
};
const static string base_str = "BTC,ETH,USD,USDT";
const static vector<string> base_syms = split(',',base_str);
int xmain( const arg_v &args )
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


  cout << endl;
  auto todos = mk_todo();

  auto trade = find_match( todos );
  if(!get<2>(trade)) {
    cout << "Nothing to do!" << endl;
    return 0;
  };
  cout
    << "trade: " 
    << get<0>(trade) 
    << get<1>(trade) 
    << get<2>(trade) 
    <<  endl;
  xact_limit(
      get<0>(trade),
      get<1>(trade),
      get<2>(trade),
      "USDT"
      );
  auto stime=time(0);
  do {
    if(!bittrex::dump_orders()) 
      break;
    market_l m = bittrex::get_market(get<3>(trade));
    cout << m << endl;
    auto ctime=time(0)-stime;
    if(ctime<15)
      sleep(3);
    else
      bittrex::cancel_orders();
  }while(0);
  cout << "all orders resolved" << endl;
  return 0;
};
int main( int argc, char** argv )
{
  try
  {
    arg_v args( argv+1, argv+argc );
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
