#define LOG_SPDLOG
//#define LOG_FMT
#define LOG_PRINTF
#include "log.h"

#include <unistd.h>
#include <signal.h>

sig_atomic_t run = 1;

void cb_sig(int sig)
{
  log_info("got signal %d\n", sig);
  run = 0;
}


struct S
{
  int a;
  int b;
};

#include <iostream>
std::ostream& operator<<(std::ostream& os, const S& rhs)
{
  return os << rhs.a << "\n" << rhs.b;
}

int main()
{
  log_init("", "/tmp/out.log", log_level_debug);

  log_debug("hei %d\n", 3);
  log_debug("hei {}\n", 4);
  //log_debug("hei %d\n", 5);
  //log_debug("hei\n");
  //log_debug("hei {}\n", 3);

  signal(SIGINT, cb_sig);


  while(run)
  {
    log_debug("loopityloop\n");

    log_error_once("loopityloop\n");

    //S s = { .a = 1, .b = 2, };
    //log_debug("s: {}\n", s);

    usleep(100000);
  }
}
