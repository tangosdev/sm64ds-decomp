#ifndef MWCPLUSLIB_H
#define MWCPLUSLIB_H

/*
 * Types of the MWCC C++ runtime, the role MSL's MWCPlusLib.h plays: the
 * element ctor/dtor pointers the array helpers take (__cxa_vec_new,
 * __cxa_vec_ctor, __cxa_vec_cleanup, func_02073300, func_02073534) and the
 * exception index table the EH runtime searches (func_02071844,
 * func_02072dac, func_02072ed0). The type names are this tree's; MSL's own
 * spellings are not recorded in the cartridge.
 *
 * Plain C, shared by the C++ array helpers and the C EH runtime files. The
 * parameters are unnamed on purpose: a named parameter in a shared
 * declaration costs every including C++ file an @NNN number.
 */

/* An element constructor or destructor, called with the element's address. */
typedef void (*ctor_t)(void *);
typedef void (*dtor_t)(void *);

/* The array helpers the compiler emits for `new T[n]` / deleting one. */
#ifdef __cplusplus
extern "C" {
#endif
void __cxa_vec_cleanup(void *, unsigned int, unsigned int, dtor_t);
void __cxa_vec_ctor(void *, unsigned int, unsigned int, ctor_t, dtor_t);
#ifdef __cplusplus
}
#endif

/* An exception index entry: start address, size with the inline flag in
 * bit 0, then the action table, or the table itself when inline (see
 * func_02072dac). */
struct EHIndexEntry {
    unsigned int start;
    unsigned int sizeFlags;
    int table;
};

/* The action-table cursor also carries the index bounds filled by
 * func_02071844. func_02072dac uses the first three words as the cursor and
 * writes indexBegin/indexEnd at +0x0c/+0x10. */
typedef struct ActionIterator {
    int mFuncStart;                  /* 0x00 */
    unsigned char *mActionTable;     /* 0x04 */
    unsigned char *mAction;          /* 0x08 */
    struct EHIndexEntry *indexBegin; /* 0x0c */
    struct EHIndexEntry *indexEnd;   /* 0x10 */
} ActionIterator;

#ifndef SM64DS_PLATFORM_PC
typedef char
    ActionIterator_indexBegin_must_be_at_0x0c[(int)&((ActionIterator *)0)->indexBegin == 0x0c ? 1
                                                                                              : -1];
typedef char
    ActionIterator_indexEnd_must_be_at_0x10[(int)&((ActionIterator *)0)->indexEnd == 0x10 ? 1 : -1];
typedef char ActionIterator_must_be_0x14_bytes[sizeof(ActionIterator) == 0x14 ? 1 : -1];
#endif

#endif /* MWCPLUSLIB_H */
