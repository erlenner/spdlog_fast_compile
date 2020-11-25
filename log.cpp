#define LOG_SPDLOG
#define LOG_FMT
#define LOG_PRINTF
#define LOG_PATH ""
#define LOG_CPP
#include "log.h"


#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

void log_str_impl_spdlog(const std::string& str, spdlog::level::level_enum lvl, const char* log_path, spdlog::source_loc src_info)
{
  static std::shared_ptr<spdlog::logger> logger;

  static bool first_run = true;
  if (first_run)
  {
    static std::vector<spdlog::sink_ptr> sinks;

    static auto stdout_logger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    stdout_logger->set_level(spdlog::level::debug);
    sinks.push_back(stdout_logger);

    if (strlen(log_path) > 0)
    {
      static auto file_logger = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_path);
      file_logger->set_level(spdlog::level::debug);
      sinks.push_back(file_logger);
    }

    logger = std::make_shared<spdlog::logger>("main", sinks.begin(), sinks.end());

    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);

    //spdlog::set_level(spdlog::level::debug);
    spdlog::cfg::load_env_levels();

    first_run = false;
  }

  logger->log(src_info, lvl, str);
}

void log_str_spdlog(const std::string& str, log_level_t lvl, const char* log_path, src_info_t&& src_info)
{
  switch(lvl)
  {
    case log_level_t::debug:
      log_str_impl_spdlog(str, spdlog::level::debug, log_path, spdlog::source_loc{src_info.file_name, src_info.line_number, src_info.function_name});
      break;
  }
}
