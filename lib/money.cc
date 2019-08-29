#include <money.hh>

std::ostream &coin::operator<<(std::ostream&lhs, coin::money_t const&rhs)
{
	char buf[128];
	snprintf(buf,sizeof(buf),"%16.8f ",rhs.get());
	return lhs<<buf;
}


