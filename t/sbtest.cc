#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <iomanip>
#include <fstream>

using namespace std;

class fd_streambuf : public streambuf
{
  int overflow(int c = EOF )
  {
    char tmp[1];
    tmp[0]=(c&0xff);
    write(1,tmp,1);
    write(2,tmp,1);
    return traits_type::to_int_type( c );
  };
};
fd_streambuf my_str;
int main( int argc, char **argv )
{
  int fd=open("logfile.log",O_WRONLY|O_APPEND|O_CREAT,0600);
  dup2(fd,2);
  close(fd);
  cout << (void*)cout.rdbuf() << endl;
  cout << (void*)cerr.rdbuf() << endl;
  cout.rdbuf(&my_str);
  cerr.rdbuf(&my_str);
  cout << (void*)cerr.rdbuf() << endl;

  cout << argc << endl;
  for(int i=0;i<argc;i++)
    cout << argv[i++] << endl;

  cerr << argc << endl;
  for(int i=0;i<argc;i++)
    cout << argv[i++] << endl;

  return 0;
}
