//cpp
// @symbol _ZN5Model13LoadTexAndPalER8BMD_File
#include "Model.h"
extern "C" {
extern u32 data_020a4bcc;
extern u32 data_020a4bd8;
void Crash();
}

struct GX {
    static void BeginLoadTexPltt();                          /* _ZN2GX16BeginLoadTexPlttEv */
    static void LoadTexPltt(const void *src, u32 addr, u32 size);
    static void EndLoadTexPltt();
};

void Model::LoadTexAndPal(BMD_File &file)
{
    u32 i;
    u32 ret;
    u32 sz;
    BMD_Palette *p;
    u32 poff;
    u32 j;
    BMD_Texture *t;
    u32 off;

    i = 0;
    if (i < file.numTextures) {
        off = i;
        do {
            t = (BMD_Texture *)((char *)file.textures + off);
            sz = t->size;
            if (((t->flags >> 26) & 7) == 5) {
                ret = LoadCompressedTextureToVram(t->data, sz, t->data + sz);
            } else {
                ret = LoadTextureToVram(t->data, sz);
            }
            t->flags = (t->flags & ~0xffff) | ((ret >> 3) & 0xffff);
            i++;
            off += sizeof(BMD_Texture);
        } while (i < file.numTextures);
    }

    GX::BeginLoadTexPltt();

    j = 0;
    if (j < file.numPalettes) {
        poff = j;
        do {
            p = (BMD_Palette *)((char *)file.palettes + poff);
            sz = p->size;
            if (data_020a4bcc + sz > data_020a4bd8) Crash();
            if (sz <= 8) {
                GX::LoadTexPltt(p->data, data_020a4bcc, sz);
                p->vramOffset = data_020a4bcc;
                data_020a4bcc = data_020a4bcc + ((sz + 7) & 0xfff8);
            } else {
                data_020a4bd8 = data_020a4bd8 - ((sz + 0xf) & 0xfff0);
                GX::LoadTexPltt(p->data, data_020a4bd8, sz);
                p->vramOffset = data_020a4bd8;
            }
            j++;
            poff += sizeof(BMD_Palette);
        } while (j < file.numPalettes);
    }

    GX::EndLoadTexPltt();
}
