#define LOG_SPDLOG
//#define LOG_FMT
#define LOG_PRINTF
#define LOG_PATH "/tmp/out.log"
#include "log.h"


int main()
{
  log_debug("hei %d", 4);
  log_debug("hei %d", 5);
  //log_debug("hei {}", 3);
}
