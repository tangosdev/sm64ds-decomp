#ifndef BMD_FILE_H
#define BMD_FILE_H

#include "types.h"

/* The BMD0 model file, as the game reads it in place.
 *
 * LAYOUT is read out of the matched code, not out of format docs:
 *
 *   * Model::Virtual10 pins numBones at 0x04 and bones at 0x08, and
 *     Model::HideMaterial indexes bones with a << 6, which pins the bone
 *     record at 0x40 bytes with the per-bone material-ID list at +0x34.
 *   * Model::LoadTexAndPal walks textures (0x14-byte records) through
 *     0x14/0x18 and palettes (0x10-byte records) through 0x1c/0x20, reading
 *     the data pointer at +0x04, the byte size at +0x08, and writing the
 *     VRAM offset it obtains back into the record.
 *   * Model::HideMaterial reaches materials through ModelComponents and
 *     ORs bit 31 into the word at +0x24 of a 0x30-byte record; the header's
 *     own 0x30-stride array at 0x24/0x28 is the same record type.
 *   * Model::UpdateFileOffsets relocates every pointer above plus the ones
 *     still named unk_*, which is where the "is a pointer" evidence for
 *     each slot comes from.
 *
 * Field NAMES are inferred from that behaviour and cannot change codegen.
 * Offsets and strides are pinned by the bytes. Records keep explicit
 * padding for the parts no matched function touches yet.
 */

struct BMD_Bone {
    u32 unk_00;
    void *unk_04;          /* 0x04 - file-relative, fixed up on load */
    u8 pad_08[0x2c];
    u8 *materialIds;       /* 0x34 - per-bone material index list */
    void *unk_38;          /* 0x38 - file-relative, fixed up on load */
    u32 unk_3c;
};

struct BMD_Material {
    void *unk_00;          /* 0x00 - file-relative, fixed up on load */
    u8 pad_04[0x20];
    u32 flags;             /* 0x24 - bit 31 hides the material */
    u8 pad_28[0x8];
};

struct BMD_Texture {
    void *unk_00;          /* 0x00 - file-relative, fixed up on load */
    char *data;            /* 0x04 */
    u32 size;              /* 0x08 */
    u32 unk_0c;
    u32 flags;             /* 0x10 - format in bits 26..28; VRAM offset >> 3
                                     packed into the low 16 bits on load */
};

struct BMD_Palette {
    void *unk_00;          /* 0x00 - file-relative, fixed up on load */
    char *data;            /* 0x04 */
    u32 size;              /* 0x08 */
    u32 vramOffset;        /* 0x0c - written by Model::LoadTexAndPal */
};

struct BMD_File {
    u32 scaleShift;            /* 0x00 */
    u32 numBones;              /* 0x04 */
    struct BMD_Bone *bones;    /* 0x08 - 0x40-byte records */
    u32 unk_0c;                /* 0x0c - count of the 8-byte records below */
    void *unk_10;              /* 0x10 - 8-byte records {count, ptr to 0x10-byte subrecords} */
    u32 numTextures;           /* 0x14 */
    struct BMD_Texture *textures;   /* 0x18 */
    u32 numPalettes;           /* 0x1c */
    struct BMD_Palette *palettes;   /* 0x20 */
    u32 numMaterials;          /* 0x24 */
    struct BMD_Material *materials; /* 0x28 */
    void *unk_2c;              /* 0x2c - fixed up on load */
    u32 unk_30;                /* 0x30 - guards the block below */
    void *unk_34;              /* 0x34 - block of four pointers, all fixed up */
};

typedef char BMD_Bone_size_must_be_0x40[sizeof(struct BMD_Bone) == 0x40 ? 1 : -1];
typedef char BMD_Material_size_must_be_0x30[sizeof(struct BMD_Material) == 0x30 ? 1 : -1];
typedef char BMD_Texture_size_must_be_0x14[sizeof(struct BMD_Texture) == 0x14 ? 1 : -1];
typedef char BMD_Palette_size_must_be_0x10[sizeof(struct BMD_Palette) == 0x10 ? 1 : -1];

#endif /* BMD_FILE_H */
