#define LOG_SPDLOG
//#define LOG_FMT
#define LOG_PRINTF
#include "log.h"


int main()
{
  log_debug("hei %d", 4);
  log_debug("hei %d", 5);
  //log_debug("hei {}", 3);
}
