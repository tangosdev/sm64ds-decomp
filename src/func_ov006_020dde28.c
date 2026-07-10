/* func_ov006_020dde28 — bump per-entry timer (stride 0x1c array at 0x4660);
 * when it reaches the table threshold for the current frame index, advance
 * the frame index (mod 4) and reset the timer.
 * Data: data_ov006_0212e32c = u16 threshold table. */

typedef unsigned short u16;
typedef unsigned char u8;

extern u16 data_ov006_0212e32c[];

typedef struct {
    char _pad0[0x12];
    u16 timer;   /* +0x12 */
    char _pad1[4];
    u8 frame;    /* +0x18 */
    char _pad2[3];
} Entry; /* 0x1c */

typedef struct {
    char _pad0[0x4660];
    Entry entries[16];
} Work;

void func_ov006_020dde28(char* c, int index)
{
    Work* w = (Work*)c;
    w->entries[index].timer++;
    if (w->entries[index].timer < data_ov006_0212e32c[w->entries[index].frame])
        return;
    w->entries[index].frame++;
    w->entries[index].frame &= 3;
    w->entries[index].timer = 0;
}
