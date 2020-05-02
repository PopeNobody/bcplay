
#ifndef util_hh
#define util_hh util_hh

#include <coinfwd.hh>
#include <iostream>
#include <fcntl.h>
#include <string>


namespace util {
  using std::streambuf;
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

  template<typename itr_t>
    inline string join(char c, itr_t b, itr_t e)
    {
      string res;
      if(b==e)
        return res;
      res=*b++;
      if(b==e)
        return res;
      while(b!=e) {
        res+=c;
        res+=*b++;
      };
      return res;
    };
  template<typename cont_t>
    inline string join(char c, cont_t cont)
    {
      return join(c,cont.begin(),cont.end());
    };
  inline vector<string> split( char sep, const string &str )
  {
    vector<string> res;
    auto s=str.begin();
    for( auto b(s), e(str.end()); b!=e; b++ ) {
      if( *b == sep )
      {
        res.push_back(string(s,b));
        while(b!=e && *b == sep)
          ++b;
        s=b;
      };
    };
    res.push_back(string(s,str.end()));

    return res;
  }
  //   	template<typename cont_t, typename itr_t>
  //   	cont_t split(char sep, itr_t b, itr_t e)
  //   	{
  //   		cont_t res;
  //   		if(b!=e) {
  //   			do {
  //   				auto s=find(b,e,sep);
  //   				res.push_back(string(b,s));
  //   				b=s;
  //   				if(b!=e)
  //   					++b;
  //   			} while(b!=e);
  //   		};
  //   		return res;
  //   	};
  //   	template<typename cont_t>
  //   	cont_t split(char sep, c_str str)
  //   	{
  //   		return split<cont_t>(sep,str.begin(),str.end());
  //   	};
  template<typename itr_t, typename val_t>
    bool contains(itr_t b, itr_t e, const val_t &val){
      while(b!=e) {
        if( *b++ == val )
          return true;
      }
      return false;
    };
  template<typename con_t, typename val_t>
    bool contains(const con_t &con, const val_t &val) {
      return contains(begin(con),end(con),val);
    };
  string strip(const string &str);
  bool exists(const char *path);
  string read_file(const char *path);
  int xrename(const char *ofn, const char *nfn);
  int xdup2(int fd, int ofd);
  int xdup(int fd);
  int xopen(const char *fn, int flags, int mode=0644);
  int xclose(int fd);
  int open_log(const std::string &fn);
};

#endif

