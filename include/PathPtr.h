#ifndef PATHPTR_H
#define PATHPTR_H
#include "types.h"

struct Vector3;

/* One entry of the level's path table: a 6-byte header naming the run of nodes
   that makes up a single path. The node positions live in a second flat table
   of 6-byte (three s16) records, indexed from firstNode. */
struct PathDef {
    u16 firstNode;          /* 0x000 */
    u8  numNodes;           /* 0x002 */
    u8  unk_003;            /* 0x003 */
    u8  unk_004;            /* 0x004 */
    u8  flags;              /* 0x005 - 0x80 marks a closed path */
};

/* A handle on one path. FromID builds it by indexing the definition table;
   everything else reads through def. */
struct PathPtr {
    struct PathDef *def;    /* 0x000 */
    s32 unk_004;            /* 0x004 */
#ifdef __cplusplus
    /* methods */
    /* DECLARED, NEVER DEFINED HERE, and that is the point: an implicit or
       inline-defined constructor would be synthesised into every constructing
       TU, and src/game/actors/PathPtr/_ZN7PathPtrC1Ev.cpp needs the compiler to emit the
       constructor as its own function instead. See notes/ctor-migration.md
       section 2. The ROM body is two zero stores at 0x0203ad74, 0x10 bytes --
       exactly this declaration plus the init list below it. */
    PathPtr();
    unsigned int Loops() const;
    unsigned int NumNodes() const;
    void GetNode(Vector3 &node, unsigned int idx) const;
#endif
};

typedef char PathDef_size_must_be_0x6[sizeof(struct PathDef) == 0x6 ? 1 : -1];
typedef char PathPtr_size_must_be_0x8[sizeof(struct PathPtr) == 0x8 ? 1 : -1];

#endif
