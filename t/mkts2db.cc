#include <fcntl.h>
#include <iostream>
#include <json.hh>
#include <sqlite/connection.hpp>
#include <sqlite/execute.hpp>
#include <sqlite/query.hpp>
#include <time.h>
#include <util.hh>
#include <web_api.hh>

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

int mkts2db()
{
  using namespace sqlite;
  connection con( "market.db" );
  try
  {
    execute( con,
             "Create Table market( "
             "   curr        text     ,  "
             "   prod        text     ,  "
             "   ask         real     ,  "
             "   bid         real     ,  "
             "   high        real     ,  "
             "   low         real     ,  "
             "   last        real     ,  "
             "   buys        real     ,  "
             "   sells       real     ,  "
             "   prevday     real     ,  "
             "   volume      real     ,  "
             "   basevolume  real     ,  "
             "   timestamp   TEXT        "
             ")",
             true );
  }
  catch ( exception& ex )
  {
    cout << "table already existed" << endl;
  };
  json data = run_query( gms_url );
  time_t tod = time( 0 );
  string sql = "INSERT INTO market values ( ?,?,?,?,?,?,?,?,?,?,?,?,? )";
  execute ins( con, sql );
  for ( json j : data )
  {
    double last, bid, ask, high, low, basevolume, volume;
    double buys, sells, prevday;
    string date, name;
#define decode( x, y, z )                                                 \
  if ( j.at( #x ).is_null() )                                             \
  {                                                                       \
    y = z;                                                                \
  }                                                                       \
  else                                                                    \
  {                                                                       \
    y = j.at( #x );                                                       \
  };

#define do_decode( x, y, z )                                              \
  do                                                                      \
  {                                                                       \
    decode( x, y, z )                                                     \
  } while ( 0 )
    do_decode( MarketName, name, "" );
    string curr, prod;
    auto res = util::split( '-', name );
    curr = res[ 0 ];
    prod = res[ 1 ];
    do_decode( TimeStamp, date, time( 0 ) );
    do_decode( Bid, bid, 0 );
    do_decode( Ask, ask, 0 );
    do_decode( High, high, 0 );
    do_decode( Low, low, 0 );
    do_decode( Volume, volume, 0 );
    do_decode( BaseVolume, basevolume, 0 );
    do_decode( Last, last, 0 );
    do_decode( PrevDay, prevday, 0 );
    ins % curr % prod % date;
    ins % last % bid % ask % high % low % basevolume % volume;
    ins % buys % sells % prevday;
    ins();
    ins.clear();
  };
  ins.emit();
  execute( con, "VACUUM;", true );
  return 0;
}
int main()
{

  try
  {
    return mkts2db();
  }
  catch ( std::exception const& e )
  {
    std::cout << "EXCEPTION: " << e.what() << std::endl;
  }
}
