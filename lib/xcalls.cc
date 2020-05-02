#include <xcalls.hh>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <boost/lexical_cast.hpp>
#include <dbg.hh>

using namespace boost;
using namespace std;

int open_log(const string &fn)
{
  struct stat stat_buf;
  if(!stat(fn.c_str(),&stat_buf))
  {
    for(int i=100;i<999;i++)
    {
      string bfn=fn+"."+lexical_cast<string>(i);
      if(!stat(bfn.c_str(),&stat_buf))
        continue;
      if(errno!=ENOENT)
        continue;
      xrename(fn.c_str(),bfn.c_str());
      break;
    };
  };
  return xopen(fn.c_str(),O_WRONLY|O_CREAT|O_APPEND,0644);
};
int xrename(const char *ofn, const char *nfn)
{
  int res=rename(ofn,nfn);
  if(res<0)
    xcroak("rename("<<ofn<<","<<nfn<<"):" << strerror(errno));
  return res;
};
int xdup2(int fd, int ofd)
{
  int res=dup2(fd,ofd);
  if(res<0)
    assert(!"dup2 failed");
  return res;
};
int xdup(int fd)
{
  int res=dup(fd);
  if(res<0)
    assert(!"dup failed");
  return res;
};
int xopen(const char *fn, int flags, int mode)
{
  int res=open(fn,flags,mode);
  if(res<0)
    assert(!"open failed");
  return res;
};
int xclose(int fd)
{
  auto res=close(fd);
  if(res<0)
    assert(!"close failed");
  return res;
};
