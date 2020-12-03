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

std::shared_ptr<spdlog::logger> spdlog_log_init_impl(const char* log_path = "", const char *format = "") // singleton
{
  static std::shared_ptr<spdlog::logger> logger;

  static bool first_run = true;
  if (first_run)
  {
    static std::vector<spdlog::sink_ptr> sinks;

    static auto stdout_logger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    sinks.push_back(stdout_logger);

    if (strlen(log_path) > 0)
    {
      static auto file_logger = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_path);
      sinks.push_back(file_logger);
    }

    logger = std::make_shared<spdlog::logger>("main", sinks.begin(), sinks.end());

    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);

    spdlog::set_level(SPDLOG_DEFAULT_LEVEL); // default level
    spdlog::cfg::load_env_levels();

    if (strlen(format) > 0)
      spdlog::set_pattern(format);
    else
      spdlog::set_pattern(SPDLOG_DEFAULT_PATTERN);

    first_run = false;
  }

  return logger;
}

void spdlog_log_init(const char *log_path, const char *format)
{
  spdlog_log_init_impl(log_path, format);
}



void spdlog_log_str_impl(const std::string& str, spdlog::level::level_enum lvl, spdlog::source_loc src_info)
{
  static auto logger = spdlog_log_init_impl();

  logger->log(src_info, lvl, str);
}

void spdlog_log_str(const std::string& str, log_level_t lvl, log_src_info_t&& src_info)
{
  switch(lvl)
  {
    case log_level_t::error:
      spdlog_log_str_impl(str, spdlog::level::err, spdlog::source_loc{src_info.file_name, src_info.line_number, src_info.function_name});
      break;
    case log_level_t::warning:
      spdlog_log_str_impl(str, spdlog::level::warn, spdlog::source_loc{src_info.file_name, src_info.line_number, src_info.function_name});
      break;
    case log_level_t::info:
      spdlog_log_str_impl(str, spdlog::level::info, spdlog::source_loc{src_info.file_name, src_info.line_number, src_info.function_name});
      break;
    case log_level_t::debug:
      spdlog_log_str_impl(str, spdlog::level::debug, spdlog::source_loc{src_info.file_name, src_info.line_number, src_info.function_name});
      break;
  }
}

log_level_t spdlog_log_level()
{
  log_level_t ret;
  static auto logger = spdlog_log_init_impl();

  switch(logger->level())
  {
    case spdlog::level::err:
      ret = log_level_t::error;
      break;
    case spdlog::level::warn:
      ret = log_level_t::warning;
      break;
    case spdlog::level::info:
      ret = log_level_t::info;
      break;
    case spdlog::level::debug:
    default:
      ret = log_level_t::debug;
      break;
  }

  return ret;
}
