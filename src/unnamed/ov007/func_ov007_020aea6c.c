#include "types.h"
#include "Ov007MenuState.h"
extern char* func_ov007_020c3df4(int a, int b);
extern int func_ov007_020c9388(int a);
extern void func_ov007_020ae810(char* c);

/* The 0x2c-byte record this function allocates; names as in
 * func_ov007_020b5bf0, func_ov007_020ae810 and func_ov007_020ae834. The
 * halfword at +0x02 and the word at +0x28 (the third argument;
 * func_ov007_020b4c04 passes a data_ov007_020d78f8 row) have no reader that
 * names them, so they stay raw. */
typedef struct Ov007Item {
    u16 id; /* 0x00: func_ov007_020b4c04's index */
    char pad_02[2];
    Ov007MenuState *state;          /* 0x04: func_ov007_020c9388(1) */
    s32 posX;                       /* 0x08: Q12 */
    s32 posY;                       /* 0x0c: Q12 */
    s32 fade;                       /* 0x10: Q12 */
    s32 slideOut;                   /* 0x14: Q12 */
    s32 wiggleTimer;                /* 0x18 */
    s32 wiggleFrames;               /* 0x1c */
    s32 ready;                      /* 0x20: func_ov007_020ae834 */
    struct Ov007ItemLayout *layout; /* 0x24: the 10-byte layout row */
    char pad_28[4];
} Ov007Item;

typedef char Ov007Item_size_must_be_0x2c[sizeof(Ov007Item) == 0x2c ? 1 : -1];

/* Allocates an item: id a, layout row b, c at +0x28; position 0, fade and
 * slideOut at 0x1000, flagged ready; then seeds its position from the
 * layout's home (func_ov007_020ae810). o keeps the char * spelling of the
 * allocator; `item` is the typed view. */
char* func_ov007_020aea6c(short a, void* b, void* c)
{
    char* o = func_ov007_020c3df4(0, 0x2c);
    Ov007Item *item = (Ov007Item *)o;
    item->state = (Ov007MenuState *)func_ov007_020c9388(1);
    item->id = a;
    *(short*)(o+2) = 0;
    item->layout = b;
    *(void**)(o+0x28) = c;
    item->posY = 0;
    item->posX = item->posY;
    item->ready = 1;
    item->fade = 0x1000;
    item->slideOut = 0x1000;
    item->wiggleTimer = 0;
    func_ov007_020ae810(o);
    return o;
}
