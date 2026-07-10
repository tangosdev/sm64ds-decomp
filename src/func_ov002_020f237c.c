/* func_ov002_020f237c — scroll sub-screen BG layers using a 4-short state
 * block pointed to by this+0xd4: y-- , x++ , then push offsets to HW.
 * Callees: SetSubBg0Offset, SetSubBg1Offset.
 */

typedef signed short s16;

struct ScrollState {
    s16 x0;  /* 0x0 */
    s16 x1;  /* 0x2 */
    s16 y0;  /* 0x4 */
    s16 y1;  /* 0x6 */
};

struct Obj {
    char _pad0[0xd4];
    struct ScrollState* scroll; /* 0xd4 */
};

extern void SetSubBg0Offset(int a, int b);
extern void SetSubBg1Offset(int a, int b);

void func_ov002_020f237c(struct Obj* self)
{
    *(s16*)(((long long)(int)((char*)self->scroll + 2)) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
    self->scroll->x0 += 1;
    SetSubBg0Offset(self->scroll->x0, self->scroll->y0);
    SetSubBg1Offset(self->scroll->x1, self->scroll->y1);
}
