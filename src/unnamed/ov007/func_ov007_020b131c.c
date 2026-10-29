#include "Ov007Work.h"

extern int func_ov007_020c0638(void* p, int a, int b, int d);
extern int* data_ov007_0210342c[];

/* The 0x38-byte BG layer record func_ov007_020c0aa8 allocates; only the
 * slots this file clears. func_ov007_020c076c writes (scroll + fineScroll)
 * >> 12 to the layer's BG scroll register; func_ov007_020c0638 sets
 * fineScroll to the sub-tile remainder of the offset it rebuilds the
 * screen at. */
typedef struct Ov007BgLayer {
    char pad_00[0x20];
    int fineScrollX; /* 0x20: Q12 */
    int fineScrollY; /* 0x24 */
    int scrollX;     /* 0x28: Q12 */
    int scrollY;     /* 0x2c */
} Ov007BgLayer;

/* The 0xc-byte layer slot func_ov007_020adc08 returns; names as in that
 * file. */
typedef struct Ov007LayerSlot {
    int state;           /* 0x00 */
    Ov007BgLayer *layer; /* 0x04 */
} Ov007LayerSlot;

typedef char Ov007BgLayer_size_must_be_0x30[sizeof(Ov007BgLayer) == 0x30 ? 1 : -1];

/* Resets the scroll of layer slots 6 and 1 and rebuilds both screens from
 * their sources at a y offset of -0xc0 pixels (func_ov007_020c0638). */
void func_ov007_020b131c(void){
    Ov007Work *work = (Ov007Work *)data_ov007_0210342c[0];
    Ov007BgLayer *a = work->layerSlots[6]->layer;
    Ov007BgLayer *b = work->layerSlots[1]->layer;
    a->scrollX = 0;
    a->scrollY = 0;
    a->fineScrollY = 0;
    a->fineScrollX = a->fineScrollY;
    b->scrollX = 0;
    b->scrollY = 0;
    b->fineScrollY = 0;
    b->fineScrollX = b->fineScrollY;
    func_ov007_020c0638(a, 0, -0xc0, 0);
    func_ov007_020c0638(b, 0, -0xc0, 0);
}
