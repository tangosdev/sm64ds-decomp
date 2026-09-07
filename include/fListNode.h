#ifndef FLISTNODE_H
#define FLISTNODE_H

#include "types.h"

struct fBase_c;

/* Intrusive actor-list node. The 0x0c-byte layout and constructor/destructor
 * lifetime are ROM-proven. The fLiNdBa_c spelling follows Nintendo's later
 * framework and is inferred for this 2004 build. */
struct fLiNdBa_c {
    fLiNdBa_c *prev;
    fLiNdBa_c *next;
    fBase_c *owner;

    fLiNdBa_c(fBase_c *owner_) : prev(0), next(0), owner(owner_) {}
    ~fLiNdBa_c();
};

typedef char fLiNdBa_c_size_must_be_0x0c[
    sizeof(fLiNdBa_c) == 0x0c ? 1 : -1];

/* Intrusive process-list node. The 0x10-byte layout and destructor lifetime are
 * ROM-proven. The fLiNdBaPr_c spelling follows Nintendo's later framework and
 * is inferred for this 2004 build. */
struct fLiNdBaPr_c {
    fLiNdBaPr_c *prev;
    fLiNdBaPr_c *next;
    fBase_c *owner;
    u16 currentPriority;
    u16 nextPriority;

    ~fLiNdBaPr_c();
};

typedef char fLiNdBaPr_c_size_must_be_0x10[
    sizeof(fLiNdBaPr_c) == 0x10 ? 1 : -1];

#endif
