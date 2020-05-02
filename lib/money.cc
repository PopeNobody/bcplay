#include <money.hh>

int coin::money_t::get_width() const {
	return 17;
};
ostream &coin::money_t::stream(ostream&lhs, int ind) const
{
//   	char buf[128];
//   	snprintf(buf,sizeof(buf),"%16.8f ",get());
  auto width = lhs.width();
  lhs.width(0);
  ostringstream fmt;
#if 0
  fmt
    << setw(5) << width
    << setw(3) << ""
    << setprecision(7) << fixed
    << setw(width-8) 
#else
  fmt
    << setprecision(8) << fixed
    << setw(width) 
#endif
    << get();
	return lhs<<fmt.str();
}


