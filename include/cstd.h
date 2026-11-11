#ifndef SM64DS_CSTD_H
#define SM64DS_CSTD_H

/* cstd -- the helpers the game still calls by our namespace: sqrt
 * (src/cstd_sqrt.cpp) and atan2 (src/cstd_atan2.cpp), both game code with no
 * SDK name, and the assertion handler (src/cstd3.cpp). The coprocessor divide
 * routines that used to live here are NitroSDK's FX_ functions
 * (include/nitro/fx/fx_cp.h); the string routines, abs and rand are MSL's
 * (include/msl/string.h, include/msl/stdlib.h). Each is spelled as its
 * definition spells it, in plain types, so the header needs nothing from
 * types.h. Parameters stay unnamed: a name here takes an @NNN in every
 * includer. */
namespace cstd {
int sqrt(unsigned long long);
short atan2(int, int);
/* Assertion failure (src/cstd3.cpp): file, expression, function, line. */
void __assert(const char *, int, const char *, int);
}

#endif
