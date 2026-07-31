//cpp
#include "types.h"
// @symbol _ZN5Model13LoadTexAndPalER8BMD_File
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Model.h"
extern "C" {


void Crash();
u32 _ZN5Model27LoadCompressedTextureToVramEPcjPc(char *src, u32 size, char *dst);
u32 _ZN5Model17LoadTextureToVramEPcj(char *src, u32 size);
void _ZN2GX16BeginLoadTexPlttEv();
void _ZN2GX11LoadTexPlttEPKvjj(const void *src, u32 addr, u32 size);
void _ZN2GX14EndLoadTexPlttEv();

void _ZN5Model13LoadTexAndPalER8BMD_File(struct Model *self) {
    u32 i;
    u32 ret;
    u32 sz;
    char *p;
    u32 poff;
    u32 j;
    char *t;
    u32 off;

    i = 0;
    if (i < self->unk_014) {
      off = i;
      do {
        t = *(char **)((char *)&self->unk_018) + off;
        sz = *(u32 *)(t + 8);
        if (((*(u32 *)(t + 0x10) >> 26) & 7) == 5) {
            ret = _ZN5Model27LoadCompressedTextureToVramEPcjPc(
                *(char **)(t + 4), sz, *(char **)(t + 4) + sz);
        } else {
            ret = _ZN5Model17LoadTextureToVramEPcj(*(char **)(t + 4), sz);
        }
        *(u32 *)(t + 0x10) = (*(u32 *)(t + 0x10) & ~0xffff) | ((ret >> 3) & 0xffff);
        i++;
        off += 0x14;
      } while (i < self->unk_014);
    }

    _ZN2GX16BeginLoadTexPlttEv();

    j = 0;
    if (j < self->unk_01c) {
      poff = j;
      do {
        p = *(char **)((char *)&self->unk_020) + poff;
        sz = *(u32 *)(p + 8);
        if (data_020a4bcc + sz > data_020a4bd8) Crash();
        if (sz <= 8) {
            _ZN2GX11LoadTexPlttEPKvjj(*(const void **)(p + 4), data_020a4bcc, sz);
            *(u32 *)(p + 0xc) = data_020a4bcc;
            data_020a4bcc = data_020a4bcc + ((sz + 7) & 0xfff8);
        } else {
            data_020a4bd8 = data_020a4bd8 - ((sz + 0xf) & 0xfff0);
            _ZN2GX11LoadTexPlttEPKvjj(*(const void **)(p + 4), data_020a4bd8, sz);
            *(u32 *)(p + 0xc) = data_020a4bd8;
        }
        j++;
        poff += 0x10;
      } while (j < self->unk_01c);
    }

    _ZN2GX14EndLoadTexPlttEv();
}

}
