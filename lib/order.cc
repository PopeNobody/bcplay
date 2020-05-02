#include <order.hh>

coin::order_t::order_t()
{
};
coin::order_t::order_t( const data_t& data )
  : data(data)
{
};
coin::order_t::order_t( const order_t& rhs )
  : data(rhs.data)
{
};
coin::order_l::~order_l()
{
};
ostream& coin::order_l::stream( ostream& lhs, int ind ) const
{
  ostringstream tmp;
  tmp << setw(ind) << "" << "order_l {" << endl;
  for( auto &ord : *this )
  {
    ord.stream(lhs,ind+2);
  };
  tmp << setw(ind) << "" << "}" << endl;
  return lhs;
};
coin::order_t::~order_t()
{
};
ostream& coin::order_t::stream( ostream& lhs, int ind ) const
{
  lhs << setw(ind) << "" << "order: " << data.uuid << endl;
  return lhs;
};
coin::order_t &coin::order_t::operator=(const coin::order_t &rhs)
{
  data=rhs.data;
  return *this;
};
