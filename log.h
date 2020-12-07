#pragma once

#ifdef __cplusplus
#include <string>
#include <cstring>
#include <utility>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

// interface
#define log_log(lvl, fmt, ...) log_impl(fmt, lvl, __VA_ARGS__)
#define log_error(...) log_log(log_level_error, __VA_ARGS__)
#define log_warning(...) log_log(log_level_warning, __VA_ARGS__)
#define log_info(...) log_log(log_level_info, __VA_ARGS__)
#define log_debug(...) log_log(log_level_debug, __VA_ARGS__)

#define log_init log_init_impl
#define log_str log_str_impl
#define log_level log_level_impl

#define log_once(...) do{ \
  static int run = 1; \
  if (run) \
  { \
    log_log(__VA_ARGS__); \
    run = 0; \
  } \
} while(0)

#define log_error_once(...) log_once(log_level_error, __VA_ARGS__)
#define log_warning_once(...) log_once(log_level_warning, __VA_ARGS__)
#define log_info_once(...) log_once(log_level_info, __VA_ARGS__)
#define log_debug_once(...) log_once(log_level_debug, __VA_ARGS__)

typedef enum { log_level_debug, log_level_info, log_level_warning, log_level_error } log_level_t;

const char* to_string(log_level_t lvl)
{
  switch(lvl)
  {
    case log_level_error:
      return "error";
    case log_level_warning:
      return "warning";
    case log_level_info:
      return "info";
    case log_level_debug:
      return "debug";
  }
}

// struct for recording the origin of the log function call
typedef struct
{
  const char* file_name;
  const char* function_name;
  int line_number;
} log_src_info_t;
#define create_log_src_info_t() { .file_name = __FILE__, .function_name = (const char*)(__FUNCTION__), .line_number = __LINE__ }

inline log_level_t log_level_from_env()
{
  const char *env = getenv("LOG_LEVEL");
  printf("env: %s\n", env);
  log_level_t stdout_lvl = log_level_info;
  if (env)
  {
    for (int i=(int)log_level_debug; i <= (int)log_level_error; ++i)
      if (strcmp(env, to_string((log_level_t)i)) == 0)
        stdout_lvl = (log_level_t)i;
  }
  return stdout_lvl;
}

// use spdlog by default
#ifndef LOG_SPDLOG
  #define LOG_SPDLOG
#endif

#ifdef LOG_SPDLOG
  #ifdef __cplusplus
  extern "C"
  {
  #endif
    void spdlog_log_init(const char *format, const char *file_path, log_level_t file_lvl, log_level_t stdout_lvl);
    #define log_init_impl(format, file_path, file_lvl) \
    do{ \
      log_level_t stdout_lvl = log_level_from_env(); \
      spdlog_log_init(format, file_path, file_lvl, stdout_lvl); \
    }while (0)

    void spdlog_log_str(const char* str, log_level_t lvl, log_src_info_t src_info);
    #define log_str_impl spdlog_log_str

    log_level_t spdlog_log_level();
    #define log_level_impl spdlog_log_level
  #ifdef __cplusplus
  } // extern "C"
  #endif

  #if defined(LOG_FMT)
    #ifndef __cplusplus
      #error fmt formatting is only available for C++, not C
    #endif

    #include <spdlog/fmt/fmt.h>
    #include <spdlog/fmt/bundled/ostream.h>

    #define log_impl(_fmt, lvl, ...) do { \
      if (lvl >= log_level()) \
      { \
        log_src_info_t src_info = create_log_src_info_t(); \
        std::string str = fmt::format(_fmt, src_info, __VA_OPT__(,) __VA_ARGS__); \
        log_str_impl(str.c_str(), lvl, src_info); \
      } \
    } while(0)

  #elif defined(LOG_PRINTF)

    #define LOG_MAX_BUFFER_LENGTH 1024

    #define log_impl(fmt, lvl, ...) do { \
      if (lvl >= log_level()) \
      { \
        char buf[LOG_MAX_BUFFER_LENGTH]; \
        log_src_info_t src_info = create_log_src_info_t(); \
        snprintf(buf, sizeof(buf), fmt __VA_OPT__(,) __VA_ARGS__); \
        log_str_impl(buf, lvl, src_info); \
      } \
    } while(0)

  #else
    #error You need to specify either LOG_FMT or LOG_PRINTF
  #endif

#endif
