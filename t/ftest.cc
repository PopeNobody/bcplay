#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

//int nanosleep(const struct timespec *req, struct timespec *rem);


using namespace std;

const static long nsec_per_sec=100000000;
int xsleep(long secs, long nsec)
{
  timespec req;
  timespec rem;
  req.tv_nsec=nsec_per_sec*secs+nsec;
  req.tv_sec=req.tv_nsec/nsec_per_sec;
  req.tv_nsec-=req.tv_sec*nsec_per_sec;
  if(nanosleep(&req,0)<0) {
    perror("nanosleep");
    abort();
  };
  return 0;
};
int xpipe(int fds[2])
{
  int res=pipe(fds);
  if(res<0)
    abort();
  return res;
};
auto xfork()
{
  auto res=fork();
  if(res<0)
    abort();
  return res;
};
typedef int fd_t;
auto xclose(fd_t fd)
{
  auto res=close(fd);
  if(res<0)
    abort();
  return res;
};
auto xdup2(fd_t fd1, fd_t fd2)
{
  auto res=dup2(fd1,fd2);
  if(res<0)
    abort();
  return res;
};
int pid=0;
void fork_less()
{
  int pipe[2];
  xpipe(pipe);
  cout << "Forking In parent. " << getpid() << endl;
  pid=xfork();
  if(pid) {
    // in parent
    cout << "In parent. " << getpid() << endl;
    xclose(pipe[0]);
    xdup2(pipe[1],1);
    xclose(pipe[1]);
    return;
  };
  cout << "In child" << getpid() << endl;
  xdup2(pipe[0],0);
  xclose(pipe[0]);
  xclose(pipe[1]);
  char *argv[20];
  char buf[1024];
  int i=0;
  argv[i++]=(char*)"/usr/bin/less";
  argv[i++]=0;
  execve(argv[0],argv,0);
  exit(0);
};
int main(int, char**) {
  int fd=open("/dev/null",O_RDONLY);
  xdup2(fd,0);
  xclose(fd);
  xdup2(1,2);
  fork_less();
  auto stime=time(0);
  auto etime=stime+1000;
  for(auto ctime=time(0);ctime<etime;ctime=time(0))
  {
    cout << stime << " " << ctime << " " << etime << endl;
    system("find | head");
    xsleep(1,0);
    auto res=waitpid(pid,0,WNOHANG);
    cout << "res=" << res << endl;
  };
  close(1);
  return 0;
};
