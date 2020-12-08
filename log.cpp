#include <stdint.h>

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
    default:
    case log_level_info:
      return spdlog::level::info;
    case log_level_debug:
      return spdlog::level::debug;
  }
}

struct logger_t
{
  std::shared_ptr<spdlog::logger> stdout_logger;
  std::shared_ptr<spdlog::logger> file_logger;
};

const logger_t& spdlog_get_loggers(const char *format = "", const char* file_path = "")
{
  static logger_t loggers;

  static bool first_run = true;
  if (first_run)
  {

    auto stdout_logger = spdlog::stdout_color_mt("stdout");
    //spdlog::register_logger(stdout_logger);
    //spdlog::set_default_logger(stdout_logger);
    stdout_logger->set_level(spdlog::level::trace);
    loggers.stdout_logger = std::move(stdout_logger);

    if (strlen(file_path) > 0)
    {
      auto file_logger = spdlog::basic_logger_mt("file", file_path);
      file_logger->set_level(spdlog::level::trace);
      //spdlog::register_logger(file_logger);
      loggers.file_logger = std::move(file_logger);
    }

    spdlog::set_level(spdlog::level::trace);

    if (strlen(format) > 0)
      spdlog::set_pattern(format);
    else
      spdlog::set_pattern(SPDLOG_DEFAULT_PATTERN);

    first_run = false;
  }

  return loggers;
}

void spdlog_log_init(const char *format, const char *file_path)
{
  if (format == NULL)
    format = "";
  if (file_path == NULL)
    file_path = "";
  spdlog_get_loggers(format, file_path); // first call initializes logger
}

void spdlog_log_str_impl(const char* str, log_level_t lvl, log_src_info_t* src_info)
{
  static auto loggers = spdlog_get_loggers();

  spdlog::source_loc spdlog_src_info = spdlog::source_loc{ src_info->file_name, src_info->line_number, src_info->function_name };

  auto spdlog_lvl = to_spdlog(lvl);

  if (src_info->write_stdout)
    loggers.stdout_logger->log(spdlog_src_info, spdlog_lvl, str);

  if ((loggers.file_logger) && (src_info->write_file))
    loggers.file_logger->log(spdlog_src_info, spdlog_lvl, str);
}

void spdlog_log_str(const char* str, log_level_t lvl, log_src_info_t* src_info)
{
  spdlog_log_str_impl(str, lvl, src_info);
}


log_level_t log_level(const char* cat)
{
  log_level_t ref_lvl = (log_level_t)-1;
  const char *env = getenv("LOG_LEVEL");
  if (env)
  {
    if ((cat == NULL) || strlen(cat) == 0)
    {
      for (int i=(int)log_level_debug; i <= (int)log_level_error; ++i)
        if (strncmp(env, to_string((log_level_t)i), strlen(to_string((log_level_t)i))) == 0)
          ref_lvl = (log_level_t)i;

      if ((int)ref_lvl == -1)
        ref_lvl = log_level_info;
    }
    else
    {
      for (const char* c = env; (c != NULL) && (c[1] != '\0'); c = strchr(c+1, ','))
      {
        if (*c == ',')
          ++c;

        int cat_len = 0;
        for (const char *i = c; *i != '\0'; ++i)
          if ((*i == '*') || (*i == '\0'))
            cat_len = i - c;

        if (strncmp(c, cat, cat_len) == 0)
        {
          const char *next_colon = strchr(c, ':');
          const char *next_comma = strchr(c, ',');

          if ((next_colon != NULL) && (next_colon[1] != '\0')
            && ((next_comma == NULL) || (next_comma[1] == '\0') || (next_comma > next_colon)))
          {
            for (int i=(int)log_level_debug; i <= (int)log_level_error; ++i)
              if (strncmp(next_colon+1, to_string((log_level_t)i), strlen(to_string((log_level_t)i))) == 0)
                ref_lvl = (log_level_t)i;
          }
        }
      }

      if ((int)ref_lvl == -1)
        ref_lvl = log_level("");
    }
  }

  return ref_lvl;
}
