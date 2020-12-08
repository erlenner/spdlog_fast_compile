#pragma once

#ifdef __cplusplus
#include <string>
#include <cstring>
#include <utility>
#include <cassert>
#else
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
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
    default:
      return "unknown";
  }
}

// struct for recording the origin of the log function call
typedef struct
{
  const char* file_name;
  const char* function_name;
  int line_number;
  bool write_stdout;
  bool write_file;
} log_src_info_t;
#define create_log_src_info_t() { \
  .file_name = __FILE__, \
  .function_name = (const char*)(__FUNCTION__), \
  .line_number = __LINE__, \
  .write_stdout = false, \
  .write_file = false, \
}

#ifdef __cplusplus
extern "C"
{
#endif
  log_level_t log_level_cat(const char* cat);
  log_level_t log_level_file(const char* cat);
#ifdef __cplusplus
} // extern "C"
#endif

#define log_impl(fmt, lvl, ...) \
do { \
  static bool init = true; \
  static bool write_stdout; \
  static bool write_file; \
  if (init) \
  { \
    write_stdout = (lvl >= log_level_cat("")); \
    write_file = (lvl >= log_level_file("")); \
    init = false; \
  } \
  if (write_stdout || write_file) \
  { \
    log_src_info_t src_info = \
    { \
      .file_name = __FILE__, \
      .function_name = (const char*)(__FUNCTION__), \
      .line_number = __LINE__, \
      .write_stdout = write_stdout, \
      .write_file = write_file, \
    }; \
    log_format_impl(fmt, lvl, src_info, __VA_ARGS__); \
  } \
} while(0)

// use spdlog by default
#ifndef LOG_SPDLOG
  #define LOG_SPDLOG
#endif

#ifdef LOG_SPDLOG
  #ifdef __cplusplus
  extern "C"
  {
  #endif
    void spdlog_log_init(const char *format, const char *file_path);
    #define log_init_impl spdlog_log_init

    void spdlog_log_str(const char* str, log_level_t lvl, log_src_info_t src_info);
    #define log_str_impl spdlog_log_str
  #ifdef __cplusplus
  } // extern "C"
  #endif

  #if defined(LOG_FMT)
    #ifndef __cplusplus
      #error fmt formatting is only available for C++, not C
    #endif

    #include <spdlog/fmt/fmt.h>
    #include <spdlog/fmt/bundled/ostream.h>

    #define log_format_impl(_fmt, lvl, src_info, ...) do { \
      std::string str = fmt::format(_fmt, src_info, __VA_OPT__(,) __VA_ARGS__); \
      log_str_impl(str.c_str(), lvl, src_info); \
    } while(0)

  #elif defined(LOG_PRINTF)

    #define LOG_MAX_BUFFER_LENGTH 1024

    #define log_format_impl(fmt, lvl, src_info, ...) do { \
      char buf[LOG_MAX_BUFFER_LENGTH]; \
      snprintf(buf, sizeof(buf), fmt __VA_OPT__(,) __VA_ARGS__); \
      log_str_impl(buf, lvl, src_info); \
    } while(0)

  #else
    #error You need to specify either LOG_FMT or LOG_PRINTF
  #endif

#endif
