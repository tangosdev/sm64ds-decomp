//cpp
#include "Ov007Work.h"
#include "Ov007Settings.h"
#include "Ov007Display.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/gx_load.h"

extern "C" void *func_ov007_020b3f20(int n);
extern void MenuText_Init(void *unused, void *sheet, int baseTile, void *plane0, void *plane1,
                          int plane2, void *bank);
extern void func_ov007_020c32e8(Ov007Palette *pal, int target, unsigned int row, int skip);
extern "C" void func_ov007_020b3edc(int r0);
extern void func_ov007_020bcdb8(int idx);
extern void func_ov007_020c33d0(Ov007CharRecord *rec, int target, unsigned int offset);
extern void MenuText_LoadFontPalette(void);

/* Loads screen set `idx`'s graphics: the message planes over the main and
 * sub BG1 (set 3 adds main BG3), its characters and palettes, and for set
 * 1 the OBJ sheet (files 0x2c/0x2d), the language's BG3 characters (file
 * 0x25 + language) and the BG palette (file 0x2b). */
extern "C" void func_ov007_020bcba8(int idx) {
    Ov007Display *s;
    Ov007Work *g = WORK;
    s = g->display;

    switch (idx) {
    case 0:
        return;
    case 1: {
        void *sheet = func_ov007_020b3f20(0x25);
        void *mainBg1 = G2_GetBG1CharPtr();
        void *subBg1 = G2S_GetBG1CharPtr();
        void *messages = g->settings->bootArgs->messages;
        int langFile;
        MenuText_Init(0, sheet, 0, mainBg1, subBg1, 0, messages);

        GX_LoadOBJ(s->characters[1]->tiles, 0, 0x2000);
        func_ov007_020c32e8(s->palettes[1], 0, 0, 0);
        GX_LoadOBJ(func_ov007_020b3f20(0x2c), 0x2000, 0x2000);
        /* file 0x2d is 16-colour rows; its row 6 becomes OBJ palette 15 */
        GX_LoadOBJPltt(((const u16(*)[16])func_ov007_020b3f20(0x2d))[6], 0x1e0, 0x20);
        func_ov007_020b3edc(0x2c);
        func_ov007_020b3edc(0x2d);

        langFile = WORK->settings->language + 0x25;
        GX_LoadBG3Char(func_ov007_020b3f20(langFile), 0, 0x4000);
        GX_LoadBGPltt(func_ov007_020b3f20(0x2b), 0, 0x200);
        func_ov007_020b3edc(langFile);
        func_ov007_020b3edc(0x2b);

        func_ov007_020bcdb8(1);
        func_ov007_020c33d0(s->characters[0], 8, 0);
        func_ov007_020c32e8(s->palettes[0], 3, 0, 0);
        MenuText_LoadFontPalette();
    } break;
    case 3: {
        void *sheet = func_ov007_020b3f20(0x25);
        void *mainBg1 = G2_GetBG1CharPtr();
        void *subBg1 = G2S_GetBG1CharPtr();
        int mainBg3 = (int)G2_GetBG3CharPtr();
        void *messages = g->settings->bootArgs->messages;
        MenuText_Init(0, sheet, 0, mainBg1, subBg1, mainBg3, messages);

        func_ov007_020bcdb8(2);
        func_ov007_020c33d0(s->characters[3], 9, 0);
        func_ov007_020c32e8(s->palettes[2], 3, 0, 0);
        MenuText_LoadFontPalette();
    } break;
    case 2:
        break;
    }
}
