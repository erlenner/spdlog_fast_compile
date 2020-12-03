#pragma once

#ifdef __cplusplus

#include <string>
#include <cstring>
#include <utility>

// interface
#define log_log(lvl, fmt, ...) log_impl(fmt, lvl, create_log_src_info_t() __VA_OPT__(,) __VA_ARGS__)
#define log_error(...) log_log(log_level_t::error, __VA_ARGS__)
#define log_warning(...) log_log(log_level_t::warning, __VA_ARGS__)
#define log_info(...) log_log(log_level_t::info, __VA_ARGS__)
#define log_debug(...) log_log(log_level_t::debug, __VA_ARGS__)

#define log_init log_init_impl
#define log_str log_str_impl
#define log_level log_level_impl

#define log_once(...) do{ \
  static int throwaway = ({ log_log(__VA_ARGS__); 0; }); \
  (void)sizeof(throwaway); /*suppress unused variable warning*/ \
} while(0)

#define log_error_once(...) log_once(log_level_t::error, __VA_ARGS__)
#define log_warning_once(...) log_once(log_level_t::warning, __VA_ARGS__)
#define log_info_once(...) log_once(log_level_t::info, __VA_ARGS__)
#define log_debug_once(...) log_once(log_level_t::debug, __VA_ARGS__)

enum class log_level_t { debug, info, warning, error };

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
  void spdlog_log_init(const char *log_path = "", const char *format = "");
  #define log_init_impl spdlog_log_init

  void spdlog_log_str(const std::string& str, log_level_t lvl, log_src_info_t&& src_info);
  #define log_str_impl spdlog_log_str

  log_level_t spdlog_log_level();
  #define log_level_impl spdlog_log_level

  inline void log_impl(const char* fmt, int str_size, log_level_t lvl, log_src_info_t&& src_info)
  {
    log_str_impl(std::string(fmt), lvl, std::forward<log_src_info_t>(src_info));
  }

  #if defined(LOG_FMT)
    #include <spdlog/fmt/fmt.h>
    #include <spdlog/fmt/bundled/ostream.h>
    template<typename... Args>
    inline void log_impl(const char* fmt, int str_size, log_level_t lvl, log_src_info_t&& src_info, Args&&... args)
    {
      if (lvl >= log_level()) // keep track of level internally to avoid unneccessary formatting
      {
        log_str_impl(fmt::format(fmt, args...), lvl, std::forward<log_src_info_t>(src_info));
      }
    }
  #elif defined(LOG_PRINTF)
    template<typename... Args, int max_format_length = 10>
    inline void log_impl(const char* fmt, int str_size, log_level_t lvl, log_src_info_t&& src_info, Args&&... args)
    {
      if (lvl >= log_level())
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

        log_str_impl(std_str, lvl, std::forward<log_src_info_t>(src_info));
      }
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
