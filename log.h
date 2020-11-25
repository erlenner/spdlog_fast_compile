#ifdef __cplusplus

#include <string>
#include <cstring>
#include <utility>

enum class log_level_t { error, warning, debug };

struct src_info_t
{
  const char* file_name;
  const char* function_name;
  int line_number;
};

// use spdlog by default
#ifndef LOG_SPDLOG
  #define LOG_SPDLOG
#endif

#ifdef LOG_SPDLOG
  void log_init_spdlog(const char *log_path, const char *format);
  void log_str_spdlog(const std::string& str, log_level_t lvl, src_info_t&& src_info);

  #ifndef LOG_CPP

    #define log_init(...) log_init_spdlog(__VA_ARGS__)
    #define log(fmt, lvl, ...) log_impl(fmt, lvl, src_info_t{ .file_name = __FILE__, .function_name = static_cast<const char *>(__FUNCTION__), .line_number = __LINE__ }, __VA_ARGS__)


    #if defined(LOG_FMT)
      #include <spdlog/fmt/fmt.h>
      template<typename... Args>
      inline void log_impl(const char* str, int str_size, log_level_t lvl, src_info_t&& src_info, Args&&... args)
      {
        log_str_spdlog(fmt::format(str, args...), lvl, std::forward<src_info_t>(src_info));
      }
    #elif defined(LOG_PRINTF)
      template<typename... Args, int max_format_length = 10>
      inline void log_impl(const char* str, int str_size, log_level_t lvl, src_info_t&& src_info, Args&&... args)
      {
        std::string std_str;
        constexpr int extra_room = sizeof...(Args) * max_format_length;
        std_str.resize(str_size + extra_room);
      
        int written = snprintf(&std_str[0], std_str.size(), str, args...);
      
        if (written > std_str.size())
        {
          std_str.resize(written);
          snprintf(&std_str[0], std_str.size(), str, args...);
        }

        log_str_spdlog(std_str, lvl, std::forward<src_info_t>(src_info));
      }
    #else
      #error You need to specify either LOG_FMT or LOG_PRINTF
    #endif

    template<int str_size, typename... Args>
    inline void log_impl(const char(&str)[str_size], log_level_t lvl, src_info_t&& src_info, Args&&... args)
    {
      log_impl(str, str_size, lvl, std::forward<src_info_t>(src_info), std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline void log_impl(const char *str, log_level_t lvl, src_info_t&& src_info, const Args&... args)
    {
      const int str_size = strlen(str) + 1;
      log_impl(str, str_size, lvl, std::forward<src_info_t>(src_info), std::forward<Args>(args)...);
    }

  #endif

#endif


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
