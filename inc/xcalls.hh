#ifndef xcalls_hh
#define xcalls_hh xcalls_hh

#include <iostream>
#include <fcntl.h>
#include <string>

int xrename(const char *ofn, const char *nfn);
int xdup2(int fd, int ofd);
int xdup(int fd);
int xopen(const char *fn, int flags, int mode=0644);
int xclose(int fd);
int open_log(const std::string &fn);

#endif
