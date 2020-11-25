#ifdef __cplusplus

#include <string>
#include <cstring>
#include <utility>

enum class log_level_t { error, warning, debug };

// use spdlog by default:
#ifndef LOG_SPDLOG
  #define LOG_SPDLOG
#endif

#ifndef LOG_PATH
  #warning LOG_PATH is unset, disabling logging to file
  #define LOG_PATH ""
#endif

#ifdef LOG_SPDLOG
  void log_str_spdlog(const std::string& str, log_level_t lvl, const char* log_path = "");

  #if defined(LOG_FMT)
    #include <spdlog/fmt/fmt.h>
    template<typename... Args>
    inline void log(const char* str, int str_size, log_level_t lvl, Args&&... args)
    {
      log_str_spdlog(fmt::format(str, args...), lvl, LOG_PATH);
    }
  #elif defined(LOG_PRINTF)
    template<typename... Args, int max_format_length = 10>
    inline void log(const char* str, int str_size, log_level_t lvl, Args&&... args)
    {
      std::string std_str;
      constexpr int extra_room = sizeof...(Args) * max_format_length;
      std_str.resize(str_size + extra_room);
    
      int written = snprintf(&std_str[0], std_str.size(), str, args...);
    
      if (written >= std_str.size())
      {
        std_str.resize(written + 1);
        sprintf(&std_str[0], str, args...);
      }

      log_str_spdlog(std_str, lvl, LOG_PATH);
    }
  #else
    #error You need to specify either LOG_FMT or LOG_PRINTF
  #endif
#endif

template<int str_size, typename... Args>
inline void log(const char(&str)[str_size], log_level_t lvl, Args&&... args)
{
  log(str, str_size, lvl, std::forward<Args>(args)...);
}

template<typename... Args>
inline void log(const char *str, log_level_t lvl, const Args&... args)
{
  const int str_size = strlen(str) + 1;
  log(str, str_size, lvl, std::forward<Args>(args)...);
}

#define log_debug(str, ...) log(str, log_level_t::debug, __VA_ARGS__)

#ifdef LOG_PROTO
  #include <google/protobuf/text_format.h>

  template<typename Message, typename Logger>
  std::enable_if_t<std::is_base_of<::google::protobuf::Message, Message>::value, int>
  log_proto(const Message& msg, Logger logger)
  {
    std::string buf;
    bool success = google::protobuf::TextFormat::PrintToString(msg, &buf);

    logger(buf.c_str());

    int rc = success ? 0 : -1;

    return rc;
  }

  #define log_proto_debug(msg) log_proto(msg, [](const char* s){ log_debug(s); })
#endif

#endif
