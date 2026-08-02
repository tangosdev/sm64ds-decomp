#ifndef MAD_PIANO_RESOURCES_H
#define MAD_PIANO_RESOURCES_H

#include "types.h"

/*
 * Resource IDs, read straight out of __sinit_ov063_0211e5fc's own arguments:
 *   0x040a -> piano.bmd
 *   0x040b -> piano.kcl
 *   0x040c -> piano_attack.bca
 *
 * The three names sit consecutively in the ROM's filename table, in that
 * order, matching the consecutive IDs. The enclosing directory is read from
 * the FNT tree rather than stored as a path, so "data/enemy/piano/" is a
 * reconstruction of the tree walk, not a literal string in the image.
 *
 * The three BSS objects are eight bytes apart and are initialized by the
 * SharedFilePtr constructor veneers, then consumed by the model,
 * mesh-collision and animation loaders respectively.
 *
 * LAYOUT is pinned by the constructor body func_02017e0c -- the one all three
 * veneers (func_02017acc, func_02017b4c, _ZN13SharedFilePtr9ConstructEj)
 * tail into:
 *
 *   strh r0, [r4]        -> fileID is a halfword at 0x00
 *   strb r2, [r4, #2]    -> refCount is a byte at 0x02, zeroed
 *   str  r2, [r4, #4]    -> file is a word at 0x04, zeroed
 *
 * Offset 0x03 is never written, which is the pad, and 8 bytes is exactly the
 * spacing of the three objects. Nothing here is inferred.
 */
typedef struct SharedFilePtrLayout {
    u16 fileID;   /* 0x00 - strh; the ctor stores func_02018a24's result, not the raw ID */
    u8 refCount;  /* 0x02 - strb 0 */
    u8 pad_03;    /* 0x03 - never written */
    void *file;   /* 0x04 - str 0 */
} SharedFilePtrLayout;

typedef char SharedFilePtrLayout_SizeIs8[
    sizeof(SharedFilePtrLayout) == 8 ? 1 : -1
];

/* Readable aliases retain the current linker symbols until a verified rename. */
#define gPianoModelFile             data_ov063_0211ef80
#define gPianoCollisionFile         data_ov063_0211ef88
#define gPianoAttackAnimationFile   data_ov063_0211ef90

#define MAD_PIANO_MODEL_ASSET              0x040a
#define MAD_PIANO_COLLISION_ASSET          0x040b
#define MAD_PIANO_ATTACK_ANIMATION_ASSET   0x040c

#endif
