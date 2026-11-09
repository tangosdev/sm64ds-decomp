#ifndef SM64DS_CSTD_H
#define SM64DS_CSTD_H

/* cstd -- the runtime helpers the game still calls by our namespace: sqrt
 * (src/cstd_sqrt.cpp), atan2 (src/cstd_atan2.cpp), abs (src/cstd4.cpp), the
 * string routines (src/cstd6.cpp) and the assertion handler (src/cstd3.cpp).
 * The coprocessor divide routines that used to live here are NitroSDK's FX_
 * functions now (include/nitro/fx/fx_cp.h). Each is spelled as its
 * definition spells it, in plain types, so the header needs nothing from
 * types.h. Parameters stay unnamed: a name here takes an @NNN in every
 * includer. */
namespace cstd {
int sqrt(unsigned long long);
short atan2(int, int);
int abs(int);

unsigned int strlen(const char *);
char *strncpy(char *, const char *, unsigned int);
int strcmp(const char *, const char *);
char *strchr(const char *, char);

/* Assertion failure (src/cstd3.cpp): file, expression, function, line. */
void __assert(const char *, const char *, const char *, int);
}

#endif
