#ifndef NITRO_MI_MEMORY_H
#define NITRO_MI_MEMORY_H

/* NitroSDK's mi_memory.c (nitro/mi/memory.h), defined in
 * src/engine/mi/MI_Memory.c. Only what a header consumer needs is declared
 * here; the parameters stay unnamed, because a named parameter in a
 * declaration costs every C++ includer a compiler @NNN number. */

#ifdef __cplusplus
extern "C" {
#endif

void MI_Copy36B(const void *, void *);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_MI_MEMORY_H */
