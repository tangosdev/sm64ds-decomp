#ifndef MSL_STDARG_H
#define MSL_STDARG_H

/* MSL's variable-argument macros for ARM, as CodeWarrior's MSL_C
 * stdarg.ARM.h defines them. A va_list is a char * walking the caller's
 * stack words: va_start steps past the last named parameter, and every
 * argument slot is rounded up to a whole word. */

typedef char *va_list;

#define __fourbytealign(n) ((((unsigned long)(n)) + 3U) & ~3U)
#define __va_start(parm) ((va_list)((char *)((unsigned long)(&parm) & ~3U) + __fourbytealign(sizeof(parm))))

#define va_start(ap, parm) ((ap) = __va_start(parm))
#define va_arg(ap, type) (*(type *)((ap += __fourbytealign(sizeof(type))) - __fourbytealign(sizeof(type))))
#define va_end(ap) ((void)0)

#endif /* MSL_STDARG_H */
