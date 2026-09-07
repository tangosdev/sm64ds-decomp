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
 *     each slot comes from. It is also what shapes the display-list chain at
 *     0x0c/0x10: a count, a table of {count, pointer} groups, and 0x10-byte
 *     records whose +0x04 and +0x0c are both file-relative.
 *   * Three names here -- BMD_Material::name / textureId / paletteId, and the
 *     matching BMD_Texture::name and BMD_Palette::name -- come from the host
 *     port instead: port/ntr/bmd.cpp parses these offsets and renders Mario
 *     correctly out of them, naming his four materials mat_bm_body / _eye /
 *     _head / _head_c against textures mario_body / _eye_1 / _head. That is
 *     empirical rather than read out of matched ARM, and it is flagged as such
 *     in notes/system-provenance.md.
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

/* One 0x10-byte display-list record. Model::UpdateFileOffsets rebases the
   words at +0x04 and +0x0c, which is the evidence that both are file-relative
   offsets; the host port reads +0x08 as the byte size of the stream at +0x0c
   and feeds that stream straight to the geometry engine (port/ntr/bmd.cpp). */
struct BMD_DisplayList {
    u32 unk_00;
    void *unk_04;          /* 0x04 - file-relative, fixed up on load */
    u32 size;              /* 0x08 - bytes of packed DS commands at data */
    void *data;            /* 0x0c - file-relative, fixed up on load */
};

/* The display lists are grouped: an array of these, one per group. */
struct BMD_DisplayListGroup {
    u32 numLists;              /* 0x00 - bounds the loop over lists */
    struct BMD_DisplayList *lists;  /* 0x04 - file-relative, fixed up on load */
};

struct BMD_Material {
    char *name;            /* 0x00 - file-relative, fixed up on load */
    u32 textureId;         /* 0x04 - index into BMD_File::textures */
    u32 paletteId;         /* 0x08 - index into BMD_File::palettes */
    u8 pad_0c[0x18];
    u32 flags;             /* 0x24 - bit 31 hides the material */
    u8 pad_28[0x8];
};

struct BMD_Texture {
    char *name;            /* 0x00 - file-relative, fixed up on load */
    char *data;            /* 0x04 */
    u32 size;              /* 0x08 */
    u32 unk_0c;
    u32 flags;             /* 0x10 - format in bits 26..28; VRAM offset >> 3
                                     packed into the low 16 bits on load */
};

struct BMD_Palette {
    char *name;            /* 0x00 - file-relative, fixed up on load */
    char *data;            /* 0x04 */
    u32 size;              /* 0x08 */
    u32 vramOffset;        /* 0x0c - written by Model::LoadTexAndPal */
};

struct BMD_File {
    u32 scaleShift;            /* 0x00 */
    u32 numBones;              /* 0x04 */
    struct BMD_Bone *bones;    /* 0x08 - 0x40-byte records */
    u32 numDisplayListGroups;  /* 0x0c - bounds the loop over the table below */
    struct BMD_DisplayListGroup *displayListGroups;  /* 0x10 */
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

typedef char BMD_DisplayList_size_must_be_0x10[sizeof(struct BMD_DisplayList) == 0x10 ? 1 : -1];
typedef char BMD_DisplayListGroup_size_must_be_0x8[sizeof(struct BMD_DisplayListGroup) == 0x8 ? 1 : -1];
typedef char BMD_Bone_size_must_be_0x40[sizeof(struct BMD_Bone) == 0x40 ? 1 : -1];
typedef char BMD_Material_size_must_be_0x30[sizeof(struct BMD_Material) == 0x30 ? 1 : -1];
typedef char BMD_Texture_size_must_be_0x14[sizeof(struct BMD_Texture) == 0x14 ? 1 : -1];
typedef char BMD_Palette_size_must_be_0x10[sizeof(struct BMD_Palette) == 0x10 ? 1 : -1];

#endif /* BMD_FILE_H */
