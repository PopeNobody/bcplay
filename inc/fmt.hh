#ifndef fmt_hh
#define fmt_hh fmt_hh

#include <coinfwd.hh>
namespace fmt {
  struct can_str {
    virtual ostream &stream(ostream &lhs, int ind=0) const=0;
    virtual ~can_str();
  };
  inline ostream &operator<<( ostream &lhs, const fmt::can_str &rhs )
  {
    return rhs.stream(lhs,0);
  };
  class fp_val : public can_str
  {
    protected:
      double val;
      virtual ~fp_val ();
      fp_val(double val)
        : val(val)
      {
      };
    public:
      virtual string fmt() const=0;
      virtual int get_width() const=0;
      double get() const {
        return val;
      };
      explicit operator double()const{
        return get();
      };
      bool operator!() const
      {
        return !val;
      };
  };
  class pct_t : public fp_val  {
    public:
      pct_t(double val=0)
        : fp_val(val)
      {
      };
      template<typename lhs_t, typename rhs_t>
        pct_t( lhs_t lhs, rhs_t rhs )
        : fp_val(lhs/rhs)
        {
        };
      virtual int get_width() const;
      string fmt() const;
      pct_t &operator+=(const pct_t&rhs)
      {
        val+=rhs.val;
        return *this;
      };
      pct_t &operator-=(const pct_t&rhs)
      {
        val+=rhs.val;
        return *this;
      };
      virtual ostream &stream(ostream &lhs, int ind=0) const;
  };
  template<typename otype_t>
    inline otype_t operator*(const otype_t &lhs, const pct_t &rhs)
    {
      return lhs*rhs.get();
    };
  template<typename otype_t>
    inline otype_t operator*(const pct_t &lhs, const otype_t &rhs)
    {
      return lhs.get()*rhs;
    };
  template<typename otype_t>
    pct_t operator+(const otype_t &lhs, const pct_t &rhs)
    {
      return lhs+rhs.get();
    };
  template<typename otype_t>
    inline pct_t operator-(const otype_t &lhs, const pct_t &rhs)
    {
      return lhs-rhs.get();
    };
  template<>
    inline pct_t operator-<pct_t>(const pct_t &lhs, const pct_t &rhs)
    {
      return lhs.get()-rhs.get();
    };
  template<typename otype_t>
    inline otype_t operator/(const otype_t &lhs, const pct_t &rhs)
    {
      return lhs*(1/rhs.get());
    };
  struct nl_t {
    friend ostream &operator<<(ostream &lhs, nl_t nl)
    {
      return lhs.put('\n');
    }
  };
  const static nl_t nl;
}
#endif

