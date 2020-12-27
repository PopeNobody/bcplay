#ifndef symbol_hh
#define symbol_hh symbol_hh

#include <coinfwd.hh>
#include <array>

//   class adapt_t {
//     virtual int compare(const adapt_t &lhs, const adapt_t &rhs) = 0;
//   };
//   
//   template<typename client_t>
//   class adapt : public adapt_t
//   {
//   };
namespace coin {
	using std::string;
	using std::ostream;
  using std::array;

//     enum cmp_id {
//       cmp_eq = 1,
//       cmp_gt = 2,
//       cmp_lt = 3,
//   
//       cmp_ne = -1,
//       cmp_le = -2,
//       cmp_ge = -2
//     }; 

	class sym_t
	{
    enum { chars = 8 };
    typedef array<char,8> text_t;
    const text_t text;
		public:
		static int get_width() {
			return 8;
		};
    static const text_t &char_array(const string &str)
    {
      static text_t res;
      if(str.size()>=8)
        xthrowre( "'"<<str<<"' is too long to assign to sym_t" );
      auto pos=copy(str.begin(),str.end(),res.begin());
      std::fill(pos,res.end(),0);
      return res;
    };
    sym_t(const string &str)
      : text(char_array(str))
    {
    };
    size_t size() const {
      return strlen(&text[0]);
    };
    const char *c_str() const {
      return &text[0];
    };
    void stream(const ostream &str);
	};


  template<typename obj_t>
    const char *to_c_str(const obj_t &obj)
    {
      return obj.c_str();
    };
  template<typename lhs_t, typename rhs_t>
    int cmp(const lhs_t &lhs, const rhs_t &rhs)
    {
      return strcmp(to_c_str(lhs),to_c_str(rhs));
    };

	class sym_l : public vector<sym_t>
 	{
	};
	ostream &operator<<(ostream &lhs, const sym_t &rhs);




};


using coin::sym_t;
//   template<typename lhs_t, typename rhs_t>
//   inline int cmp(const lhs_t &lhs, const rhs_t &rhs);
//   template<>
//   inline int cmp<sym_t,sym_t>(const sym_t &lhs, const sym_t &rhs)
//   {
//     return strncmp(lhs.c_str(),rhs.c_str(),lhs.size());
//   };
template<typename lhs_t, typename rhs_t>
inline bool operator==(const lhs_t &lhs, const rhs_t &rhs)
{
  return cmp(lhs,rhs)==0;
};
template<typename lhs_t, typename rhs_t>
inline bool operator<=(const lhs_t &lhs, const rhs_t &rhs)
{
  return cmp(lhs,rhs)<=0;
};
template<typename lhs_t, typename rhs_t>
inline bool operator>=(const lhs_t &lhs, const rhs_t &rhs)
{
  return cmp(lhs,rhs)>=0;
};
template<typename lhs_t, typename rhs_t>
inline bool operator!=(const lhs_t &lhs, const rhs_t &rhs)
{
  return !(lhs==rhs);
};
template<typename lhs_t, typename rhs_t>
inline bool operator> (const lhs_t &lhs, const rhs_t &rhs)
{
  return !(lhs<=rhs);
};
template<typename lhs_t, typename rhs_t>
inline bool operator< (const lhs_t &lhs, const rhs_t &rhs)
{
  return !(lhs>=rhs);
};
inline ostream &coin::operator<<(ostream &lhs, const sym_t &rhs) {
  return lhs << rhs.c_str();
};


#endif

