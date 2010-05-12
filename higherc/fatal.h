#include <stdarg.h>

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
# define FATAL(...)       fatal(__FILE__, __LINE__, __VA_ARGS__)
#elif defined (__GNUC__)
# define FATAL(fmt...)   fatal(__FILE__, __LINE__, fmt)
#endif

/* this function ceases program execution with a exit(1)
 */
void fatal(char *file, int line, char *fmt, ...);
