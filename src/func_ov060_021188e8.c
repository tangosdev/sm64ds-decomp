/* func_ov060_021188e8 — set flag bit0 (0x13c), grow the uniform scale
 * (0x80/0x84/0x88) as timer(0x1ac) * 0x9000 / 14 + 0x1000; at timer == 0x1c
 * call func_ov060_021184bc and bump the timer. */

typedef unsigned int u32;
typedef unsigned short u16;

extern void func_ov060_021184bc(char* c);

typedef struct {
    char _pad0[0x80];
    int scaleX;    /* +0x080 */
    int scaleY;    /* +0x084 */
    int scaleZ;    /* +0x088 */
    char _pad1[0xb0];
    u32 flags;     /* +0x13c */
    char _pad2[0x6c];
    u16 timer;     /* +0x1ac */
} Work;

void func_ov060_021188e8(char* c)
{
    Work* w = (Work*)c;
    int scale;
    (*(u32*)((long long)(int)(c + 0x13c) & 0xFFFFFFFFFFFFFFFFLL)) |= 1;
    scale = (w->timer * 9 << 12) / 14 + 0x1000;
    w->scaleX = scale;
    w->scaleY = scale;
    w->scaleZ = scale;
    if (w->timer == 0x1c)
        func_ov060_021184bc(c);
    (*(u16*)((long long)(int)(c + 0x1ac) & 0xFFFFFFFFFFFFFFFFLL))++;
}
