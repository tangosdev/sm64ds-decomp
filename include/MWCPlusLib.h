#ifndef MWCPLUSLIB_H
#define MWCPLUSLIB_H

/*
 * Types of the MWCC C++ runtime, the role MSL's MWCPlusLib.h plays: the
 * element constructor/destructor pointers the array helpers take
 * (src/lib/runtime/__cxa_vec_*.cpp, func_02073300, func_02073534) and the
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

/* An exception index entry: start address, size with the inline flag in
 * bit 0, then the action table, or the table itself when inline (see
 * func_02072dac). */
struct EHIndexEntry {
    unsigned int start;
    unsigned int sizeFlags;
    int table;
};

/* The view func_02072dac fills: the action table covering a code address,
 * and the bounds of the index table func_02071844 hands it. */
struct EHTableInfo {
    int start;
    unsigned char *table;
    int action;
    struct EHIndexEntry *indexBegin;
    struct EHIndexEntry *indexEnd;
};

#endif /* MWCPLUSLIB_H */
