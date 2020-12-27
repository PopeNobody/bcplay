#include <dbg.hh>
namespace dbg 
{

//     ostream &show_dbg_msg(const char *file, unsigned line, const char *msg )
//     {
//       cerr << file << ":" << line << ":";
//       if(msg)
//         cerr << msg;
//       return cerr;
//     };
}
using namespace std;

ostream &helper::stream(ostream &lhs, const type_info &rhs)
{
  return lhs << rhs.name();
};
ostream &helper::stream(ostream &lhs, const exception &rhs)
{
  return lhs << rhs.what();
};
