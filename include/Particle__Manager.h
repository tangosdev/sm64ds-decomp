#ifndef PARTICLE__MANAGER_H
#define PARTICLE__MANAGER_H

#include "types.h"

namespace Particle {

struct System;

/* One 0x20-byte entry in the manager's effect-definition table. The table
   stride and the leading pointer are fixed by Manager::AddSystem and
   SetSelfDestructFlag; the remaining fields are not reached by either. */
struct SystemDefinition {
    struct Data {
        u32 flags;          /* 0x00 - bit 14 requests self-destruction */
    };

    Data *data;             /* 0x00 */
    u8 pad_004[0x1c];
};

struct Manager {
    /* The three manager lists are the same two-word intrusive-list heads used
       by func_0204d958/func_0204d9a0. Their element types differ, so only the
       two lists reached by AddSystem are named here. */
    struct SystemList {
        System *head;       /* 0x00 */
        s32 count;          /* 0x04 */
    };

    void *allocator;                    /* 0x00 */
    SystemList mActiveSystems;          /* 0x04 */
    SystemList mFreeSystems;            /* 0x0c */
    u8 pad_014[0x8];                    /* 0x14 - another intrusive list */
    SystemDefinition *mDefinitions;     /* 0x1c */
    void *unk_020;                      /* 0x20 */
    u16 unk_024;                        /* 0x24 */
    u16 unk_026;                        /* 0x26 */
    u16 unk_028;                        /* 0x28 */
    u16 unk_02a;                        /* 0x2a */
    u16 unk_02c;                        /* 0x2c */
    u16 unk_02e;                        /* 0x2e */
    u8 pad_030[0xc];

    System *AddSystem(int definitionID, Vector3 &position);
};

void SetSelfDestructFlag(u32 definitionID);

typedef char SystemDefinition_size_must_be_0x20[
    sizeof(SystemDefinition) == 0x20 ? 1 : -1];
typedef char Manager_size_must_be_0x3c[
    sizeof(Manager) == 0x3c ? 1 : -1];

}

#endif
