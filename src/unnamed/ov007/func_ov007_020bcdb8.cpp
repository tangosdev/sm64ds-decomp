//cpp
#include "nitro/gx/gx_load.h"
#include "Ov007Work.h"
#include "Ov007Settings.h"
#include "Ov007Display.h"

extern void func_ov007_020c33d0(Ov007CharRecord *rec, int target, unsigned int offset);
extern void func_ov007_020c32e8(Ov007Palette *pal, int target, unsigned int row, int skip);
extern "C" void *func_ov007_020b3f20(int idx);
extern "C" Ov007CharRecord *func_ov007_020c99d8(int x);
extern "C" void func_ov007_020c3544(void *p);
extern "C" void func_ov007_020b3edc(int idx);

typedef char Ov007Work_display_must_be_at_0x74[(int)&((Ov007Work *)0)->display == 0x74 ? 1 : -1];

/* Loads character record idx into the sub engine's OBJ characters and
 * palette 1 into its OBJ palette. For language 2 and up it also builds a
 * character record from file slot idx + 0x17 + 2 * (language - 2)
 * (func_ov007_020b3f20), loads it to the sub OBJ as well, copies its first
 * 0x2000 bytes to the main OBJ characters, then frees the record and
 * releases the slot. */
void func_ov007_020bcdb8(int idx) {
    Ov007Display *s = WORK->display;
    func_ov007_020c33d0(s->characters[idx], 5, 0);
    func_ov007_020c32e8(s->palettes[1], 2, 0, 0);

    int k = WORK->settings->language - 2;
    if (k < 0)
        return;

    int slot = idx + 0x17 + (k << 1);
    Ov007CharRecord *rec = func_ov007_020c99d8((int)func_ov007_020b3f20(slot));
    func_ov007_020c33d0(rec, 5, 0);
    GX_LoadOBJ(rec->tiles, 0, 0x2000);
    func_ov007_020c3544(rec);
    func_ov007_020b3edc(slot);
}
