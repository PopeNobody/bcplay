#include <fcntl.h>
#include <iostream>
#include <json.hh>
#include <sqlite/connection.hpp>
#include <sqlite/execute.hpp>
#include <sqlite/query.hpp>
#include <time.h>
#include <web_api.hh>
#include <util.hh>

using namespace std;

static const string gb_url =
         "https://bittrex.com/api/v1.1/account/getbalances?";
const static string gms_url =
         "https://bittrex.com/api/v1.1/public/getmarketsummaries?";

json run_query( const string& url )
{
  auto jpage = json::parse( web::load_hmac_page( url ) );
  if ( !jpage.at( "success" ) )
    throw runtime_error( "no success in getbalances result" );
  return jpage.at( "result" );
};

int bals2db()
{
  using namespace sqlite;
  connection con( "balance.db" );
  try
  {
    execute( con,
        "create table holding ( "
        "  market   text   ,  "
        "  balance  real   ,  "
        "  pending  real   ,  "
        "  avail    real   ,  "
        "  addr     text   ,  "
        "  utime    int       "
        "  )",
        true );
  }
  catch ( std::exception const& e )
  {
    std::cout << "EXCEPTION: " << e.what() << std::endl;
  }

  execute ins( con, "INSERT INTO holding VALUES(?,?,?,?,?,?);" );

  json data = run_query( gb_url );
  time_t tod = time( 0 );
  for ( json obj : data )
  {
    string coin = obj.at( "Currency" );
    float bal = obj.at( "Balance" );
    float ava = obj.at( "Available" );
    float pend = obj.at( "Pending" );
    string addr = "";
    auto& ref = obj.at( "CryptoAddress" );
    if ( !ref.is_null() )
      addr = ref;


    ins % coin % bal % bal % bal % addr % tod;
    ins();
    ins.clear();
  };
  return 0;
}
int main()
{

  try
  {
    return bals2db();
  }
  catch ( std::exception const& e )
  {
    std::cout << "EXCEPTION: " << e.what() << std::endl;
  }
  return -1;
}
