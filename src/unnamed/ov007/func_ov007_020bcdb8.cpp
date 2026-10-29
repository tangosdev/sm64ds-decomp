//cpp
#include "Ov007Work.h"

extern "C" void func_ov007_020c33d0(void* a, int b, int c);
extern "C" void func_ov007_020c32e8(void* c, int sel, unsigned int n, int off);
extern "C" void func_ov007_020b3f20(void* p);
extern "C" void* func_ov007_020c99d8(void);
struct GX { static void LoadOBJ(void const*, unsigned int, unsigned int); };
extern "C" void func_ov007_020c3544(void* p);
extern "C" void func_ov007_020b3edc(int r0);
extern "C" char* data_ov007_0210342c;

/* The 0x3c-byte record at the manager's +0x28; names as in
 * func_ov007_020b6a84, only the slot read here. */
struct Ov007Settings {
    char pad_00[0x3a];
    unsigned char language; /* 0x3a: func_ov007_020b6a84 seeds it from the owner-info language */
};

/* The manager's +0x74: the 0x48-byte 2D display record func_ov007_020b7594
 * allocates; names as in func_ov007_020b0244, only the tables read here. */
struct Ov007Display {
    char pad_00[0x24];
    /* 0x24: four func_ov007_020c99d8 character records; func_ov007_020c33d0
     * loads one into OBJ VRAM */
    void **characters;
    char pad_28[4];
    void **palettes; /* 0x2c: six func_ov007_020c9988 palettes */
};

typedef char Ov007Work_display_must_be_at_0x74[(int)&((Ov007Work *)0)->display == 0x74 ? 1 : -1];

/* The extern keeps the tree's char * spelling; the typed view is reached
 * through a cast macro. */
#define WORK ((Ov007Work *)data_ov007_0210342c)

/* Loads character record idx and palette 1; for language 2 and up it also
 * opens file idx + 0x17 + 2 * (language - 2) (func_ov007_020b3f20) and
 * copies 0x2000 bytes of a character record to the start of OBJ VRAM. */
extern "C" void func_ov007_020bcdb8(int idx)
{
    Ov007Display *s = WORK->display;
    func_ov007_020c33d0(s->characters[idx], 5, 0);
    func_ov007_020c32e8(s->palettes[1], 2, 0, 0);

    int k = WORK->settings->language - 2;
    if (k < 0) return;

    char* obj = (char*)(idx + 0x17 + (k << 1));
    func_ov007_020b3f20(obj);
    void* m = func_ov007_020c99d8();
    func_ov007_020c33d0(m, 5, 0);
    GX::LoadOBJ(*(void**)m, 0, 0x2000);
    func_ov007_020c3544(m);
    func_ov007_020b3edc((int)obj);
}
