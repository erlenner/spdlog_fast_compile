#pragma once

#ifdef __cplusplus

#include <string>
#include <cstring>
#include <utility>

// interface
#define log_log(fmt, lvl, ...) log_impl(fmt, lvl, create_log_src_info_t() __VA_OPT__(,) __VA_ARGS__)
#define log_error(str, ...) log_log(str, log_level_t::error, __VA_ARGS__)
#define log_warning(str, ...) log_log(str, log_level_t::warning, __VA_ARGS__)
#define log_debug(str, ...) log_log(str, log_level_t::debug, __VA_ARGS__)

#define log_init(...) log_init_impl(__VA_ARGS__)
#define log_str(...) log_str_impl(__VA_ARGS__)

enum class log_level_t { error, warning, debug };

// struct for recording the origin of the log function call
struct log_src_info_t
{
  const char* file_name;
  const char* function_name;
  int line_number;
};
#define create_log_src_info_t() log_src_info_t{ .file_name = __FILE__, .function_name = static_cast<const char *>(__FUNCTION__), .line_number = __LINE__ }

// use spdlog by default
#ifndef LOG_SPDLOG
  #define LOG_SPDLOG
#endif

#ifdef LOG_SPDLOG
  void log_init_spdlog(const char *log_path = "", const char *format = "");
  #define log_init_impl(...) log_init_spdlog(__VA_ARGS__)

  void log_str_spdlog(const std::string& str, log_level_t lvl, log_src_info_t&& src_info);
  #define log_str_impl(...) log_str_spdlog(__VA_ARGS__)

  inline void log_impl(const char* str, int str_size, log_level_t lvl, log_src_info_t&& src_info)
  {
    log_str_spdlog(std::string(str), lvl, std::forward<log_src_info_t>(src_info));
  }

  #if defined(LOG_FMT)
    #include <spdlog/fmt/fmt.h>
    template<typename... Args>
    inline void log_impl(const char* str, int str_size, log_level_t lvl, log_src_info_t&& src_info, Args&&... args)
    {
      log_str_spdlog(fmt::format(str, args...), lvl, std::forward<log_src_info_t>(src_info));
    }
  #elif defined(LOG_PRINTF)
    template<typename... Args, int max_format_length = 10>
    inline void log_impl(const char* str, int str_size, log_level_t lvl, log_src_info_t&& src_info, Args&&... args)
    {
      std::string std_str;
      constexpr int extra_room = sizeof...(Args) * max_format_length;
      std_str.resize(str_size + extra_room);

      unsigned int written = snprintf(&std_str[0], std_str.size(), str, args...);

      if (written > std_str.size())
      {
        std_str.resize(written);
        snprintf(&std_str[0], std_str.size()+1, str, args...);
      }

      log_str_spdlog(std_str, lvl, std::forward<log_src_info_t>(src_info));
    }
  #else
    #error You need to specify either LOG_FMT or LOG_PRINTF
  #endif

  template<int str_size, typename... Args>
  inline void log_impl(const char(&str)[str_size], log_level_t lvl, log_src_info_t&& src_info, Args&&... args)
  {
    log_impl(str, str_size, lvl, std::forward<log_src_info_t>(src_info), std::forward<Args>(args)...);
  }

  template<typename... Args>
  inline void log_impl(const char *str, log_level_t lvl, log_src_info_t&& src_info, const Args&... args)
  {
    const int str_size = strlen(str) + 1;
    log_impl(str, str_size, lvl, std::forward<log_src_info_t>(src_info), std::forward<Args>(args)...);
  }

  template<int str_size>
  inline void log_impl(const char(&str)[str_size], log_level_t lvl, log_src_info_t&& src_info)
  {
    log_impl(str, str_size, lvl, std::forward<log_src_info_t>(src_info));
  }

  inline void log_impl(const char *str, log_level_t lvl, log_src_info_t&& src_info)
  {
    const int str_size = strlen(str) + 1;
    log_impl(str, str_size, lvl, std::forward<log_src_info_t>(src_info));
  }

#endif

#endif
