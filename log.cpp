#define SPDLOG_EOL ""
#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#define LOG_SPDLOG
#define LOG_FMT
#define LOG_PRINTF
#define LOG_PATH ""
#include "log.h"

#define SPDLOG_DEFAULT_PATTERN "[%^%l%$: %s:%# (%!) %H:%M:%S.%e] %v" // https://spdlog.docsforge.com/v1.x/3.custom-formatting/#pattern-flags

#define SPDLOG_DEFAULT_LEVEL spdlog::level::info

spdlog::level::level_enum to_spdlog(log_level_t lvl)
{
  switch(lvl)
  {
    case log_level_error:
      return spdlog::level::err;
    case log_level_warning:
      return spdlog::level::warn;
    case log_level_info:
      return spdlog::level::info;
    case log_level_debug:
      return spdlog::level::debug;
  }
}

std::shared_ptr<spdlog::logger> spdlog_log_init_impl(const char *format = "", const char* file_path = "", spdlog::level::level_enum file_level = SPDLOG_DEFAULT_LEVEL, spdlog::level::level_enum stdout_level = SPDLOG_DEFAULT_LEVEL)
{
  static std::shared_ptr<spdlog::logger> logger;

  static bool first_run = true;
  if (first_run)
  {
    static std::vector<spdlog::sink_ptr> sinks;

    static auto stdout_logger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    sinks.push_back(stdout_logger);

    if (strlen(file_path) > 0)
    {
      static auto file_logger = std::make_shared<spdlog::sinks::basic_file_sink_mt>(file_path);
      sinks.push_back(file_logger);
      file_logger->set_level(file_level);
    }

    logger = std::make_shared<spdlog::logger>("main", sinks.begin(), sinks.end());

    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);

    spdlog::set_level(spdlog::level::trace);
    stdout_logger->set_level(stdout_level);
    //spdlog::cfg::load_env_levels(); // override stdout debug level through command line: SPDLOG_LEVEL=debug ./a.out

    if (strlen(format) > 0)
      spdlog::set_pattern(format);
    else
      spdlog::set_pattern(SPDLOG_DEFAULT_PATTERN);

    first_run = false;
  }

  return logger;
}

void spdlog_log_init(const char *format, const char *file_path, log_level_t file_lvl, log_level_t stdout_lvl)
{
  spdlog_log_init_impl(format, file_path, to_spdlog(file_lvl), to_spdlog(stdout_lvl));
}



void spdlog_log_str_impl(const char* str, spdlog::level::level_enum lvl, spdlog::source_loc src_info)
{
  static auto logger = spdlog_log_init_impl();

  logger->log(src_info, lvl, str);
}

void spdlog_log_str(const char* str, log_level_t lvl, log_src_info_t src_info)
{
  spdlog_log_str_impl(str, to_spdlog(lvl), spdlog::source_loc{src_info.file_name, src_info.line_number, src_info.function_name});
}

log_level_t spdlog_log_level()
{
  log_level_t ret;
  static auto logger = spdlog_log_init_impl();

  switch(logger->level())
  {
    case spdlog::level::err:
      ret = log_level_error;
      break;
    case spdlog::level::warn:
      ret = log_level_warning;
      break;
    case spdlog::level::info:
      ret = log_level_info;
      break;
    case spdlog::level::debug:
    default:
      ret = log_level_debug;
      break;
  }

  return ret;
}
