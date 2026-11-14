#ifndef MSL_STDARG_H
#define MSL_STDARG_H

/* The formatter's argument cursor is a char *. Each slot is sizeof(type). */
#define va_arg(ap, type) (*(type *)((ap += sizeof(type)) - sizeof(type)))

#endif
