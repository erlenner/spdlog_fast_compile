#define LOG_SPDLOG
#define LOG_FMT
#include "log.h"


#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"

int log_init_spdlog()
{
  spdlog::cfg::load_env_levels();

  return 0;
}

void log_str_spdlog(const std::string& str, log_level_t lvl)
{
  static int singleton = log_init_spdlog();
  (void)sizeof(singleton); // ignore unused singleton value

  switch(lvl)
  {
    case log_level_t::error:
      spdlog::log(spdlog::level::err, str);
      break;
    case log_level_t::warning:
      spdlog::log(spdlog::level::warn, str);
      break;
    case log_level_t::debug:
      spdlog::log(spdlog::level::debug, str);
      break;
  }
}


#define log_impl(lvl, ...) do { \
  log_init(); \
  spdlog::log(spdlog::level:: lvl, __VA_ARGS__); \
} while(0)
