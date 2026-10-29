#include "types.h"
#include "dExtModel_c.h"

/* Texture-pattern (BTP) step: for each sequence of the BTP file, find the
 * last key whose frame is <= the requested frame, then point the material
 * the sequence drives at that key's texture and palette. dExtAnmTexPat_c
 * (src/engine/model/d_ext_model.cpp) calls it with the model's
 * ModelComponents, its BTP_File and the integer frame.
 *
 * ModelComponents and the BMD views come from dExtModel_c.h and BMD_File.h.
 * BTP_File is a view of dExtAnmTexPat_c.h's (which a C file can't include)
 * with the same names where that header has them, extended with what this
 * function shows:
 *   - BTP_File +0x00 is numFrames, the frame count dExtAnmTexPat_c::SetFile
 *     hands to SetAnimation; a frame past it crashes.
 *   - BTP_File +0x10/+0x14/+0x18 (unk_ in the header) are three parallel
 *     u16 key tables: keyFrames (searched for the last frame <= frame),
 *     keyTexIndices (into texRecords) and keyPalIndices (into palRecords).
 *   - a 0xc-byte seqRecords entry is { materialId (func_02046d50 resolves
 *     it by name; 0xffff is unresolved), name, numKeys, firstKey }.
 *   - texRecords/palRecords are func_02046d50's 8-byte { resourceId, name }
 *     records; resourceId is the BMD texture or palette index.
 *   - the 0x30-byte runtime material (MaterialState here; dExtModel_c.h types
 *     ModelComponents::materials as BMD_Material, but its builder in
 *     Model4.cpp lays it out differently) gets texImageParam at 0x1c (the
 *     file material's word ORed with the texture's flags) and texPlttBase
 *     at 0x20 (the palette's VRAM offset >> 3 for a 4-colour texture,
 *     format 2 in flags bits 26..28, else >> 4: the NitroSDK palette base
 *     units). */
struct BtpSeqRecord {
    u16 materialId; /* 0x0 */
    u16 pad2;
    u16 pad4;
    u16 pad6;
    u16 numKeys;  /* 0x8 */
    u16 firstKey; /* 0xa */
};
struct BtpResRecord {
    u16 resourceId; /* 0x0 */
    u16 pad2;
    char *name; /* 0x4 */
};
struct MaterialState {
    char pad_00[0x1c];
    u32 texImageParam; /* 0x1c */
    u32 texPlttBase;   /* 0x20 */
    char pad_24[0xc];
};
struct BTP_File {
    u16 numFrames;                   /* 0x00 */
    u16 numTexRecords;               /* 0x02 */
    struct BtpResRecord *texRecords; /* 0x04 */
    u16 numPalRecords;               /* 0x08 */
    u16 pad_0a;
    struct BtpResRecord *palRecords; /* 0x0c */
    u16 *keyFrames;                  /* 0x10 */
    u16 *keyTexIndices;              /* 0x14 */
    u16 *keyPalIndices;              /* 0x18 */
    u16 numSeqRecords;               /* 0x1c */
    u16 pad1e;
    struct BtpSeqRecord *seqRecords; /* 0x20 */
};
#ifndef SM64DS_PLATFORM_PC
typedef char BtpSeqRecord_size_must_be_0xc[sizeof(struct BtpSeqRecord) == 0xc ? 1 : -1];
typedef char MaterialState_size_must_be_0x30[sizeof(struct MaterialState) == 0x30 ? 1 : -1];
typedef char BTP_File_size_must_be_0x24[sizeof(struct BTP_File) == 0x24 ? 1 : -1];
#endif

extern void Crash(void);
void func_02046bbc(struct ModelComponents *model, struct BTP_File *file, unsigned int frame) {
    int i;
    if (frame >= file->numFrames) {
        Crash();
    }
    for (i = 0; i < file->numSeqRecords; i++) {
        int j;
        u16 *p;
        struct BtpSeqRecord *e;
        struct MaterialState *mat;
        int k;
        int v0;
        u16 idx0;
        struct BMD_Texture *tex;
        int flags;
        e = &file->seqRecords[i];
        if (e->materialId == 0xffff) {
            Crash();
        }
        mat = &((struct MaterialState *)model->materials)[e->materialId];
        j = e->numKeys - 1;
        p = ((&file->keyFrames[e->firstKey]) + e->numKeys) - 1;
        for (;;) {
            if ((*p) <= frame) {
                break;
            }
            p--;
            j--;
            if (j < 0) {
                Crash();
            }
        }

        k = j + e->firstKey;
        idx0 = file->keyTexIndices[k];
        v0 = idx0;
        idx0 = file->texRecords[v0].resourceId;
        tex = &model->modelFile->textures[idx0];
        mat->texImageParam = model->modelFile->materials[e->materialId].texImageParam;
        mat->texImageParam |= tex->flags;
        flags = (tex->flags >> 26) & 7;
        if (file->numPalRecords != 0) {
            u16 t = file->keyPalIndices[k];
            int c = file->palRecords[t].resourceId;
            if (c != (-1)) {
                struct BMD_Palette *pal = &model->modelFile->palettes[c];
                if (flags == 2) {
                    mat->texPlttBase = pal->vramOffset >> 3;
                } else {
                    mat->texPlttBase = pal->vramOffset >> 4;
                }
            }
        }
    }
}
