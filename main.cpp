#define LOG_SPDLOG
#define LOG_FMT
//#define LOG_PRINTF
#include "log.h"


int main()
{
  //log_init("/tmp/out.log", "[source %s] [function %!] [line %#] %v");

  log_debug("hei %d\n", 3);
  log_debug("hei {}\n", 4);
  //log_debug("hei %d\n", 5);
  //log_debug("hei\n");
  //log_debug("hei {}\n", 3);
}
