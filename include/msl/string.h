#ifndef MSL_STRING_H
#define MSL_STRING_H

/* The Metrowerks MSL C string routines the game calls (MSL's string.h),
 * defined in src/cstd6.cpp. The ROM keeps them in MSL_Common string.c's
 * order reversed -- strchr, strcmp, strncpy, strcpy, strlen, with the
 * unreferenced strcat/strncat/strncmp dead-stripped -- as pret pokediamond's
 * arm9/asm/MSL_Common_string.s lists them; the prototypes follow pret's
 * arm9/lib/MSL_C/include/string.h. size_t is unsigned int here.
 *
 * Parameters stay unnamed: a named parameter in a declaration takes a
 * compiler @NNN number in every includer. */

#ifdef __cplusplus
extern "C" {
#endif

char *strcpy(char *, const char *);
unsigned int strlen(const char *);
char *strncpy(char *, const char *, unsigned int);
int strcmp(const char *, const char *);
char *strchr(const char *, int);

#ifdef __cplusplus
}
#endif

#endif /* MSL_STRING_H */
