#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#define LOG_LEVEL_CRIT 6
#define LOG_LEVEL_QUIET 5
#define LOG_LEVEL_ERROR 4
#define LOG_LEVEL_WARNING 3
#define LOG_LEVEL_NOTICE 2
#define LOG_LEVEL_DEBUG 1

#define LOG_COLOR_RED 1
#define LOG_COLOR_GREEN 2
#define LOG_COLOR_YELLOW 3
#define LOG_COLOR_BLUE 4
#define LOG_COLOR_MAGENTA 5
#define LOG_COLOR_CYAN 6
#define LOG_COLOR_WHITE 7

#define LOG_COLOR_CODE_RESET "\x1B[0m"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LOG_DEACTIVATE


static int log_level = LOG_LEVEL_WARNING;
static int log_default_color = 0;
static int log_time = 1;
static FILE *_fp = NULL; 

static void log_set_level(int level)
{
  log_level = level;
}

static void log_set_default_color(int default_color)
{
  log_default_color = default_color;
}

static void log_set_time(int t)
{
  log_time = t;
}

static void log_set_fp(FILE *fp)
{
  _fp = fp;
}

static char *__get_time_now(void)
{
  time_t now;
  struct tm *now_tm;
  static char time_str[30];

  now = time(NULL);
  now_tm = localtime(&now);
  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", now_tm);
  return time_str;
}

static char __log_level_to_char(int level)
{
  switch(level) {
  case LOG_LEVEL_DEBUG: return 'D';
  case LOG_LEVEL_CRIT: return 'C';
  case LOG_LEVEL_ERROR: return 'E';
  case LOG_LEVEL_WARNING: return 'W';
  case LOG_LEVEL_NOTICE: return 'N';
  case LOG_LEVEL_QUIET: return 'Q';
  }

  return '*';
}

static char *__log_color_to_ascii_code(int color)
{
  switch(color) {
  case LOG_COLOR_RED: return "\x1B[31m";
  case LOG_COLOR_GREEN: return "\x1B[32m";
  case LOG_COLOR_YELLOW: return "\x1B[33m";
  case LOG_COLOR_BLUE: return "\x1B[34m";
  case LOG_COLOR_MAGENTA: return "\x1B[35m";
  case LOG_COLOR_CYAN: return "\x1B[36m";
  case LOG_COLOR_WHITE: return "\x1B[37m";
  }

  return LOG_COLOR_CODE_RESET;
}

static void __log_func(int level, char *fmt, ...)
{
  va_list ap;  
  time_t now; 
  int color_code = -1; 

  if (level < log_level) return;
  if (!_fp) { _fp = stderr; }

  if (log_default_color) {
    switch(level) {
    case LOG_LEVEL_DEBUG: color_code = LOG_COLOR_GREEN; break;
    case LOG_LEVEL_CRIT: color_code = LOG_COLOR_RED; break;
    case LOG_LEVEL_ERROR: color_code = LOG_COLOR_YELLOW; break;
    case LOG_LEVEL_WARNING: color_code = LOG_COLOR_MAGENTA; break;
    case LOG_LEVEL_NOTICE: color_code = LOG_COLOR_BLUE; break;
    }
    fprintf(_fp, "%s", __log_color_to_ascii_code(color_code)); 
  }
  
  if (log_time) {
    fprintf(_fp, "[%s] [%c] ", __get_time_now(), __log_level_to_char(level));
  } else {
    fprintf(_fp, "[%c] ",  __log_level_to_char(level));
  }
  
  va_start(ap, fmt);\
  vfprintf(_fp, fmt, ap);\
  va_end(ap);\

  if (log_default_color) {
    fprintf(_fp, LOG_COLOR_CODE_RESET);
  }

}

  
static void __log_func_color(int color, int level, char *fmt, ...)
{
  va_list ap;

  if (level < log_level) return;
  if (!_fp) { _fp = stderr; }

  fprintf(_fp, "%s", __log_color_to_ascii_code(color)); 

  va_start(ap, fmt);
  __log_func(level, fmt, ap);
  va_end(ap);

  fprintf(_fp, LOG_COLOR_CODE_RESET); 
}


#define log_debug(f, ...)   \
  __log_func(LOG_LEVEL_DEBUG,   f, ##__VA_ARGS__)
#define log_debug_c(c, f, ...)				\
  __log_func_color(c, LOG_LEVEL_DEBUG,   f, ##__VA_ARGS__)
#define log_crit(f, ...)          \
  __log_func(LOG_LEVEL_CRIT,    f, ##__VA_ARGS__)
#define log_crit_c(c, f, ...)				\
  __log_func_color(c, LOG_LEVEL_CRIT,   f, ##__VA_ARGS__)
#define log_error(f, ...)   \
  __log_func(LOG_LEVEL_ERROR,   f, ##__VA_ARGS__)
#define log_error_c(c, f, ...)				\
  __log_func_color(c, LOG_LEVEL_ERROR,   f, ##__VA_ARGS__)
#define log_warning(f, ...)   \
  __log_func(LOG_LEVEL_WARNING, f, ##__VA_ARGS__)
#define log_warning_c(c, f, ...)				\
  __log_func_color(c, LOG_LEVEL_WARNING,   f, ##__VA_ARGS__)
#define log_notice(f, ...)   \
  __log_func(LOG_LEVEL_NOTICE,  f, ##__VA_ARGS__)
#define log_notice_c(c, f, ...)				\
  __log_func_color(c, LOG_LEVEL_NOTICE,   f, ##__VA_ARGS__)
#define log_enter(f, ...)   \
  __log_func_color(LOG_COLOR_RED, LOG_LEVEL_NOTICE,  f, ##__VA_ARGS__)
#define log_leave(f, ...)   \
  __log_func_color(LOG_COLOR_RED, LOG_LEVEL_NOTICE,  f, ##__VA_ARGS__)

static void log_test()
{
  log_debug("debug:%d\n",123);
  log_crit("crit\n");
  log_error("error\n");
  log_warning("warning\n");
  log_notice("notice\n");
}

#else // LOG_DEACTIVATE

#define log_set_level(level) {(void)0;}
#define log_set_default_color(default_color) {(void)0;}
#define log_set_time(t) {(void)0;}
#define log_set_fp(fp) {(void)0;} 
#define log_debug(f, ...) {(void)0;}
#define log_debug_c(c, f, ...) {(void)0;}
#define log_crit(f, ...) {(void)0;}
#define log_crit_c(c, f, ...) {(void)0;}
#define log_error(f, ...) {(void)0;}
#define log_error_c(c, f, ...) {(void)0;}
#define log_warning(f, ...) {(void)0;}
#define log_warning_c(c, f, ...) {(void)0;}
#define log_notice(f, ...) {(void)0;}
#define log_notice_c(c, f, ...) {(void)0;}
#define log_enter(f, ...) {(void)0;}
#define log_leave(f, ...) {(void)0;}
#define log_test() {(void)0;}
#endif // LOG_DEACTIVATE
  
#ifdef __cplusplus
} // extern "C"
#endif
  
#endif // _LOG_H_
