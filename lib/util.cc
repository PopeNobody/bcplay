#include <util.hh>
#include <xcalls.hh>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
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
string util::read_file(const char *name)
{
	ifstream file(name);
	std::stringstream buf;
	buf << file.rdbuf();
	string res=buf.str();
	cout << "read " << res.length() << " bytes" << endl;
	return move(res);
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
int util::open_log(const string &in_fn)
{
  struct stat stat_buf;
  string fn(in_fn);
  for(int i=100;i<1000;i++)
  {
    if(stat(fn.c_str(),&stat_buf)<0 && errno==ENOENT)
      return xopen(fn.c_str(),O_WRONLY|O_CREAT|O_APPEND,0644);
    
    fn=in_fn+"."+lexical_cast<string>(i);
    xrename(in_fn.c_str(),fn.c_str());
  };
  xthrowre("clean your log dir, you have 1000 of them!");
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
    assert(!"dup2 failed");
  return res;
};
int util::xdup(int fd)
{
  int res=dup(fd);
  if(res<0)
    assert(!"dup failed");
  return res;
};
int util::xopen(const char *fn, int flags, int mode)
{
  int res=open(fn,flags,mode);
  if(res<0)
    assert(!"open failed");
  return res;
};
int util::xclose(int fd)
{
  auto res=close(fd);
  if(res<0)
    assert(!"close failed");
  return res;
};
