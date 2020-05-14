#include <util.hh>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <dbg.hh>

string util::strip(const string &str)
{
	auto b(str.begin());
	auto e(str.end());
	if(b==e)
		return str;
	while(b!=e && isspace(*--e))
		;
	while(b!=e && isspace(*b))
		++b;
	return string(b,++e);
};
string util::read_file(const string &name)
{
  xtrace("reading: " << name);
	ifstream file(name);
  if(!file)
    xthrowre("failed to read file " << name << ":" << strerror(errno));
	std::stringstream buf;
	buf << file.rdbuf();
	string res=buf.str();
	cout << "read " << res.length() << " bytes" << endl;
	return res;
};
ssize_t util::write_file(const string &name, const string &text)
{
  xtrace("writing: " << name);
  ofstream ofile;
  ofile.open(name,ios::app);
  if(!ofile)
    xthrowre("open:"<<name<<": "<<strerror(errno));
  ofile<<text;
  if(!ofile)
    xthrowre("error writing "<<name);
  return text.length();
};
bool util::exists(const char *name)
{
	struct stat buf;
	if(stat(name,&buf)<0){
		return false;
	};
	return true;
};

util::fd_streambuf::~fd_streambuf()
{
  if(cout.rdbuf()==this)
    cout.rdbuf(0);
  if(cerr.rdbuf()==this)
    cerr.rdbuf(0);
};

using namespace boost;
using namespace std;
using namespace util;
int util::open_log(const string &in_fn, bool save)
{
  string fn(in_fn);
  if( save ) {
    struct stat stat_buf;
    for(int i=100;i<1000;i++)
    {
      int res=stat(fn.c_str(),&stat_buf);
      if( res && errno==ENOENT ) {
        if(in_fn != fn )
          xrename(in_fn.c_str(),fn.c_str());
        return xopen(in_fn.c_str(),O_WRONLY|O_CREAT|O_APPEND,0644);
      };
      fn=in_fn+"."+lexical_cast<string>(i);
    };
    xthrowre("clean your log dir, you have 1000 of them!");
  } else {
    return xopen(in_fn.c_str(),O_WRONLY|O_CREAT|O_APPEND,0644);
  };
};
int util::xrename(const char *ofn, const char *nfn)
{
  int res=rename(ofn,nfn);
  if(res<0)
    xcroak("rename("<<ofn<<","<<nfn<<"):" << strerror(errno));
  return res;
};
int util::xdup2(int fd, int ofd)
{
  int res=dup2(fd,ofd);
  if(res<0)
    xthrowre("xdup2:" << fd << "," << ofd << ":" << strerror(errno));
  return res;
};
int util::xdup(int fd)
{
  int res=dup(fd);
  if(res<0)
    xthrowre("xdup2:" << fd << ":" << strerror(errno));
  return res;
};
int util::xopen(const char *fn, int flags, int mode)
{
  int res=open(fn,flags,mode);
  if(res<0)
    xthrowre("open:" << (fn?fn:"<null>") << ":" << strerror(errno));
  return res;
};
int util::xclose(int fd)
{
  auto res=close(fd);
  if(res<0)
    xthrowre("xclose:" << fd << ":" << strerror(errno));
  return res;
};
void util::split_stream(const string &logname) {
  int fd=xopen(logname.c_str(), O_WRONLY|O_CREAT,0777);
  static util::fd_streambuf obuf(1,fd);
  static util::fd_streambuf ibuf(2,fd);
  cout.rdbuf(&obuf);
  cerr.rdbuf(&ibuf);
};
ostream &operator<<(ostream &lhs, const std::exception &rhs)
{
  return lhs<<rhs.what();
};
ostream &operator<<(ostream &lhs, const type_info &rhs)
{
  return lhs<<demangle(typeid(rhs).name());
};
