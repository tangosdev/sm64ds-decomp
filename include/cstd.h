#ifndef SM64DS_CSTD_H
#define SM64DS_CSTD_H

/* cstd -- the runtime helpers the game calls by namespace: the division-unit
 * routines (src/cstd.cpp), sqrt (src/cstd_sqrt.cpp), atan2
 * (src/cstd_atan2.cpp), abs (src/cstd4.cpp) and the string routines
 * (src/cstd6.cpp). Each is spelled as its definition spells it, in plain
 * types: Fix12i, s32 and s16 are int, int and short, so the header needs
 * nothing from types.h. Parameters stay unnamed: a name here takes an @NNN in
 * every includer. */
namespace cstd {
int mod(int, int);
int div(int, int);
long long ldiv(int, int);
int fdiv(int, int);
int fdiv_result();
long long ldiv_result();
void fdiv_async(int, int);
void reciprocal_async(int);

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
