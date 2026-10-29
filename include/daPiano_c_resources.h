#ifndef DAPIANO_C_RESOURCES_H
#define DAPIANO_C_RESOURCES_H

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
 * SharedFilePtr constructor veneers (func_02017acc, func_02017b4c,
 * _ZN13SharedFilePtr9ConstructEj, all tailing into func_02017e0c), then
 * consumed by the model, mesh-collision and animation loaders respectively.
 * Their layout is SharedFilePtr's own (include/SharedFilePtr.h).
 */

/* Readable aliases for the three ov063 BSS handles, defined by
 * d_a_piano.cpp under the configured data_ov063_* names so mwcc emits the
 * static initializer that constructs them. */
#define gPianoModelFile             data_ov063_0211ef80
#define gPianoCollisionFile         data_ov063_0211ef88
#define gPianoAttackAnimationFile   data_ov063_0211ef90

#define MAD_PIANO_MODEL_ASSET              0x040a
#define MAD_PIANO_COLLISION_ASSET          0x040b
#define MAD_PIANO_ATTACK_ANIMATION_ASSET   0x040c

#endif
