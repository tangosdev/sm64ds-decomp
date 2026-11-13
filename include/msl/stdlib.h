#ifndef MSL_STDLIB_H
#define MSL_STDLIB_H
#ifndef __cplusplus
#define abs _Z3absi
#define rand _Z4randv
#endif

/* The Metrowerks MSL C stdlib routines the game calls: abs (MSL arith.c,
 * defined in src/cstd4.cpp) and rand (MSL rand.c, defined in src/cstd6.cpp).
 * Prototypes as in pret pokediamond's arm9/lib/MSL_C/include/stdlib.h.
 *
 * Parameters stay unnamed: a named parameter in a declaration takes a
 * compiler @NNN number in every includer. */

#if defined(_MSC_VER) && defined(__cplusplus)
extern "C" {
#endif

int abs(int);
int rand(void);

#if defined(_MSC_VER) && defined(__cplusplus)
}
#endif

#endif /* MSL_STDLIB_H */
