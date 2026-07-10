/* func_ov006_020f6a78 — once at least 3 items are queued, mark slot
 * (total*2 - 1 - count) done (stride 0x18 array at 0x51a8), bump count;
 * when count reaches the per-mode threshold (data_ov006_0212e93c),
 * set state 5 (mode 1) or 4 (otherwise). */

typedef unsigned short u16;
typedef unsigned char u8;

extern int data_ov006_0212e93c[];

typedef struct {
    char _pad0[0x13];
    u8 done;     /* +0x13 */
    char _pad1[4];
} Slot; /* 0x18 */

typedef struct {
    char _pad0[0x51a8];
    Slot slots[23];   /* 0x51a8 */
    char _pad1[8];
    int state;        /* 0x53d8 */
    char _pad2[0xa];
    short ready;      /* 0x53e6 */
    short count;      /* 0x53e8 */
    u16 total;        /* 0x53ea */
    char _pad3[0x1e];
    u8 mode;          /* 0x540a */
} Work;

void func_ov006_020f6a78(char* c)
{
    Work* w = (Work*)c;
    u8 mode;
    if (w->ready < 3)
        return;
    w->slots[w->total * 2 - 1 - w->count].done = 1;
    (*(short*)((long long)(int)(c + 0x53e8) & 0xFFFFFFFFFFFFFFFFLL))++;
    mode = w->mode;
    if (w->count < data_ov006_0212e93c[mode])
        return;
    if (mode == 1)
        w->state = 5;
    else
        w->state = 4;
}
