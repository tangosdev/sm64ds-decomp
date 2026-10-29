#ifndef PARTICLE__MANAGER_H
#define PARTICLE__MANAGER_H

#include "types.h"

struct Matrix4x3;

namespace Particle {

struct System;
struct Behavior;

/* One 0x20-byte entry in the manager's effect-definition table, filled by
   func_0204a17c from the particle resource. Each pointer addresses a record
   inside the resource; an optional record's pointer is 0 when the matching
   bit of data->flags is clear. */
struct SystemDefinition {
    struct Data {
        u32 flags;          /* 0x00 - bit 14 requests self-destruction */
        u32 callbackParam;  /* 0x04 - written by edStarKiraCallback_c */
        u8 pad_008[0x20];
        u16 callbackValue;  /* 0x28 - loaned to a live System */
    };

    Data *data;          /* 0x00 - 0x38-byte record */
    void *optionalA;     /* 0x04 - 0xc-byte record, flags bit 8 */
    void *optionalB;     /* 0x08 - 0xc-byte record, flags bit 9 */
    void *optionalC;     /* 0x0c - 8-byte record, flags bit 10 */
    void *optionalD;     /* 0x10 - 0xc-byte record, flags bit 11 */
    void *child;         /* 0x14 - 0x14-byte child record, flags bit 16 */
    Behavior *behaviors; /* 0x18 - one entry per flags bit 24..29 set */
    u16 numBehaviors;    /* 0x1c */
    u8 pad_01e[2];
};

struct Manager {
    /* The three manager lists are the same two-word intrusive-list heads used
       by func_0204d958/func_0204d9a0. Element types differ. AddSystem names
       the system lists; the third list is the free-particle pool. */
    struct SystemList {
        System *head; /* 0x00 */
        s32 count;    /* 0x04 */
    };

    /* func_0204a4c8 fills this with `b` nodes of 0x44 bytes, and func_02049d60
       hands it to a system. The node is not Particle::Element (0x30). */
    struct FreeList {
        void *head; /* 0x00 */
        s32 count;  /* 0x04 */
    };

    /* One 0x14-byte texture slot. func_0204a17c points `resource` at the
       texture's record in the particle resource and decodes its size;
       func_0204a0dc and func_0204a028 store the VRAM addresses that
       Texture::AllocTexVram and Texture::AllocPalVram return. */
    struct TextureEntry {
        struct Resource {
            struct Params {
                u32 format : 4;    /* 5 allocates 4x4-texel VRAM, 2 low palette VRAM */
                u32 widthLog : 4;  /* width is 8 << widthLog */
                u32 heightLog : 4; /* height is 8 << heightLog */
                u32 : 20;
            };

            u8 pad_000[4];
            Params params;     /* 0x04 */
            u32 texelSize;     /* 0x08 - texels follow this 0x20-byte header */
            u32 paletteOffset; /* 0x0c - from the start of this record */
            u32 paletteSize;   /* 0x10 - 0 when there is no palette */
            u8 pad_014[8];
            u32 size; /* 0x1c - the whole record */
        };

        Resource *resource;      /* 0x00 */
        u32 texelAddress;        /* 0x04 */
        u32 paletteAddress;      /* 0x08 */
        Resource::Params params; /* 0x0c - copy of resource->params */
        u16 width;               /* 0x10 */
        u16 height;              /* 0x12 */
    };

    char *(*allocator)(int);        /* 0x00 */
    SystemList mActiveSystems;      /* 0x04 */
    SystemList mFreeSystems;        /* 0x0c */
    FreeList mFreeParticles;        /* 0x14 */
    SystemDefinition *mDefinitions; /* 0x1c */
    TextureEntry *mTextures;        /* 0x20 */
    u16 mNumDefinitions;            /* 0x24 */
    u16 mNumTextures;               /* 0x26 */
    u16 mNumSystems;                /* 0x28 - size of the System pool */
    u16 mNumParticles;              /* 0x2a - size of the particle pool */
    u16 unk_02c;                    /* 0x2c */
    u16 unk_02e;                    /* 0x2e */
    /* Base POLYGON_ATTR bits the draw routines OR into each polygon;
       SysTracker::Initialise sets 0x8000. */
    u32 mPolygonAttr; /* 0x30 */
    /* func_02049ee8 stores each system here before drawing it. */
    System *mCurrentSystem; /* 0x34 */
    /* func_02049ee8's matrix argument, read by the draw routines. */
    Matrix4x3 *mDrawMatrix; /* 0x38 */

    System *AddSystem(int definitionID, Vector3 &position);
};

void SetSelfDestructFlag(u32 definitionID);

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char SystemDefinition_size_must_be_0x20[
    sizeof(SystemDefinition) == 0x20 ? 1 : -1];
typedef char TextureEntry_size_must_be_0x14[sizeof(Manager::TextureEntry) == 0x14 ? 1 : -1];
typedef char Manager_size_must_be_0x3c[
    sizeof(Manager) == 0x3c ? 1 : -1];
#endif

}

#endif
