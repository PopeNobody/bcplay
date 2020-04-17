#include <iostream>
#include <web_api.hh>
#include <sys/wait.h>


using namespace std;

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
void fork_less()
{
  int pipe[2];
  xpipe(pipe);
  cout << "about to fork" << endl;
  if(xfork()) {
    // in parent
    cout << "In parent. " << getpid() << endl;
    xclose(pipe[0]);
    xdup2(pipe[1],1);
    xclose(pipe[1]);
    sleep(1);
    return;
  };
  xdup2(pipe[0],0);
  xclose(pipe[0]);
  xclose(pipe[1]);
  char *argv[2];
  char buf[1024];
  sprintf(buf,"ls -l /proc/%d/fd/* /proc/%d/fd/*",getppid(),getpid());
  system(buf);
  sleep(100);
  argv[0]=(char*)"/usr/bin/less";
  argv[1]=0;
  execve(argv[0],argv,0);
  exit(0);
};
int main(int, char**) {
  fork_less();
  string_t text;
  for(;;){
    char ch = cin.get();
    if( ch == EOF )
      break;
    text+=ch;
  };
  cout << web::pp_json(text) << endl;
  close(1);
  return 0;
};
