/* func_ov006_020d61dc — bump per-slot timer (stride 0x10 array at 0x6280);
 * every 5 ticks reset it and bump level; at level >= 4 set state=2, timer=0x20. */

typedef unsigned short u16;
typedef unsigned char u8;

typedef struct {
    char _pad0[8];
    u16 timer;   /* +0x08 */
    char _pad1[2];
    u8 state;    /* +0x0c */
    char _pad2[2];
    u8 level;    /* +0x0f */
} Slot; /* 0x10 */

typedef struct {
    char _pad0[0x6280];
    Slot slots[16];
} Work;

void func_ov006_020d61dc(char* c, int index)
{
    Work* w = (Work*)c;
    w->slots[index].timer++;
    if (w->slots[index].timer < 5)
        return;
    w->slots[index].timer = 0;
    w->slots[index].level++;
    if (w->slots[index].level >= 4) {
        w->slots[index].state = 2;
        w->slots[index].timer = 0x20;
    }
}
