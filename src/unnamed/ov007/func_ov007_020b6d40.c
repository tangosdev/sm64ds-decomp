#include "types.h"
#include "Ov007Work.h"
extern char *data_ov007_0210342c;
extern int data_ov007_020d7e28[];
extern int data_ov007_020d7e38[];
extern int data_ov007_020d7e50[];
extern int data_ov007_020d7ec8[];
extern u8 data_ov007_020d7a24[];

extern int func_ov007_020b7594(int a, int b);
extern void func_ov007_020b7320(char *c, int a, int b);
extern int func_ov007_020c99d8(int x);
extern char *func_ov007_020c9988(int a);
extern void func_ov007_020b72fc(char *c, int n);
extern int func_ov007_020c9688(int a);
extern void *func_ov007_020ade58(int self);
extern void func_ov007_020b72d8(char *c, int n);
extern int *func_ov007_020c95fc(void *a, int b, int c);
extern int func_ov007_020adc08(int type, char *obj);

/* A func_ov007_020c9988 palette (func_ov007_020c3398 builds the 8-byte
 * record); colors as in func_ov007_020ada70. */
typedef struct Ov007Palette {
    char *colors; /* 0x00: 16-color (0x20-byte) rows */
    /* 0x04: how many colors func_ov007_020c32e8 loads (it copies
     * (colorCount - off) * 2 bytes) */
    u16 colorCount;
} Ov007Palette;

/* The manager's +0x74: the 0x48-byte 2D display record func_ov007_020b7594
 * allocates; palettes and spriteBanks as in func_ov007_020ada70 and
 * func_ov007_020ade58, only the tables this function fills. Each table's
 * count sits in the word before it (func_ov007_020b7320,
 * func_ov007_020b72d8, func_ov007_020b72fc). */
typedef struct Ov007Display {
    char pad_00[0x24];
    /* 0x24: four func_ov007_020c99d8 character records; func_ov007_020c33d0
     * loads one into OBJ VRAM */
    void **characters;
    char pad_28[4];
    Ov007Palette **palettes; /* 0x2c: six func_ov007_020c9988 palettes */
    char pad_30[4];
    /* 0x34: eight func_ov007_020c95fc BG layers, each wrapped by
     * func_ov007_020adc08 into a layer slot */
    int **bgLayers;
    char pad_38[4];
    char **spriteBanks; /* 0x3c: six func_ov007_020c9688 sprite banks */
} Ov007Display;

typedef char Ov007Display_size_must_be_0x40[sizeof(Ov007Display) == 0x40 ? 1 : -1];

/* The extern keeps the tree's char * spelling; the typed view is reached
 * through a cast macro. */
#define WORK ((Ov007Work *)data_ov007_0210342c)

#pragma opt_strength_reduction off
#pragma opt_common_subs off

void func_ov007_020b6d40(void)
{
    Ov007Display *c;
    int i;
    int tmp;

    tmp = func_ov007_020b7594(0x1f, 0x1f);
    WORK->display = (Ov007Display *)tmp;
    c = WORK->display;
    func_ov007_020b7320((char *)c, 4, 6);

    for (i = 0; i < 4; i++) {
        c->characters[i] = (void *)func_ov007_020c99d8(data_ov007_020d7e28[i]);
    }

    for (i = 0; i < 6; i++) {
        c->palettes[i] = (Ov007Palette *)func_ov007_020c9988(data_ov007_020d7e38[i]);
        if (i >= 2 && i <= 5) {
            c->palettes[i]->colorCount = 0xd0;
        }
    }

    func_ov007_020b72fc((char *)c, 6);

    for (i = 0; i < 6; i++) {
        c->spriteBanks[i] = (char *)func_ov007_020c9688(data_ov007_020d7e50[i]);
    }

    for (i = 0; i < 9; i++) {
        WORK->itemNodes[i] = func_ov007_020ade58(i);
    }

    func_ov007_020b72d8((char *)c, 8);

    for (i = 0; i < 8; i++) {
        c->bgLayers[i] =
            func_ov007_020c95fc((void *)data_ov007_020d7ec8[i], (&data_ov007_020d7a24[i * 2])[0],
                                (&data_ov007_020d7a24[i * 2])[1]);
        WORK->layerSlots[i] =
            (struct Ov007LayerSlot *)func_ov007_020adc08(i, (char *)c->bgLayers[i]);
    }
}
