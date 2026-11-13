#ifndef MSL_STRING_H
#define MSL_STRING_H
#ifndef __cplusplus
#define memcmp _Z6memcmpPKvS0_j
#define memchr _Z6memchrPKvij
#define memset _Z6memsetPvij
#define memcpy _Z6memcpyPvPKvj
#define strchr _Z6strchrPKci
#define strcmp _Z6strcmpPKcS0_
#define strncpy _Z7strncpyPcPKcj
#define strcpy _Z6strcpyPcPKc
#define strlen _Z6strlenPKc
#endif

/* The Metrowerks MSL C string routines the game calls (MSL's string.h),
 * defined in src/cstd6.cpp. The ROM keeps them in MSL_Common string.c's
 * order reversed -- strchr, strcmp, strncpy, strcpy, strlen, with the
 * unreferenced strcat/strncat/strncmp dead-stripped -- as pret pokediamond's
 * arm9/asm/MSL_Common_string.s lists them; the prototypes follow pret's
 * arm9/lib/MSL_C/include/string.h. size_t is unsigned int here.
 *
 * The mem functions are mem.c's, defined in src/cstd4.cpp: memcmp, memchr,
 * memset and memcpy, mem.c's order reversed with memmove dead-stripped.
 *
 * Parameters stay unnamed: a named parameter in a declaration takes a
 * compiler @NNN number in every includer. */

#if defined(_MSC_VER) && defined(__cplusplus)
extern "C" {
#endif

void *memcpy(void *, const void *, unsigned int);
void *memset(void *, int, unsigned int);
void *memchr(const void *, int, unsigned int);
int memcmp(const void *, const void *, unsigned int);

char *strcpy(char *, const char *);
unsigned int strlen(const char *);
char *strncpy(char *, const char *, unsigned int);
int strcmp(const char *, const char *);
char *strchr(const char *, int);

#if defined(_MSC_VER) && defined(__cplusplus)
}
#endif

#endif /* MSL_STRING_H */
