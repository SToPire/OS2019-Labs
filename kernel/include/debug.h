#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef DEBUG
#define Log(format, ...) \
    printf("\33[0m[\33[1;32mLog\33[0m]\33[1;34m[%s,%d,%s]\n%*.s" format "\33[0m\n", \
       5, "", __FILE__, __LINE__, __func__, ## __VA_ARGS__)
#define Assert(cond, format, ...) \
  do { \
    if (!(cond)) { \
      printf("\33[0m[\33[1;31mAssert\33[0m]\33[1;34m[%s,%d,%s] " format "\33[0m\n", \
          __FILE__, __LINE__, __func__, ## __VA_ARGS__); \
      assert(cond); \
    } \
  } while (0)

#else

#define Log(format, ...)
#define Assert(cond, format, ...)

#endif

#endif
