#ifndef MSL_STDLIB_H
#define MSL_STDLIB_H

/* The Metrowerks MSL C stdlib routines the game calls: abs (MSL arith.c,
 * defined in src/cstd4.cpp) and rand (MSL rand.c, defined in src/cstd6.cpp).
 * Prototypes as in pret pokediamond's arm9/lib/MSL_C/include/stdlib.h.
 *
 * Parameters stay unnamed: a named parameter in a declaration takes a
 * compiler @NNN number in every includer. */

#ifdef __cplusplus
extern "C" {
#endif

int abs(int);
int rand(void);

#ifdef __cplusplus
}
#endif

#endif /* MSL_STDLIB_H */
