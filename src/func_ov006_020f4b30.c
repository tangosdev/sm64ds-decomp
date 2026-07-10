/* func_ov006_020f4b30 — once at least 2 items are ready, mark slot
 * (11 - count) done (stride 0x18 array at 0x51a8), bump count; then per
 * mode: mode 1 -> state 4 at count >= 10, else state 3/4 at count >= 8. */

typedef unsigned char u8;

typedef struct {
    char _pad0[0x13];
    u8 done;     /* +0x13 */
    char _pad1[4];
} Slot; /* 0x18 */

typedef struct {
    char _pad0[0x51a8];
    Slot slots[12];   /* 0x51a8 */
    char _pad1[0x50];
    int state;        /* 0x5318 */
    char _pad2[0xa];
    short ready;      /* 0x5326 */
    short count;      /* 0x5328 */
    char _pad3[0x12];
    u8 mode;          /* 0x533c */
} Work;

void func_ov006_020f4b30(char* c)
{
    Work* w = (Work*)c;
    u8 mode;
    if (w->ready < 2)
        return;
    w->slots[0xb - w->count].done = 1;
    (*(short*)((long long)(int)(c + 0x5328) & 0xFFFFFFFFFFFFFFFFLL))++;
    mode = w->mode;
    if (mode == 1) {
        if (w->count >= 10)
            w->state = 4;
        return;
    }
    if (w->count < 8)
        return;
    if (mode != 0)
        w->state = 3;
    else
        w->state = 4;
}
