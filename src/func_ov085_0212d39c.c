/* func_ov085_0212d39c — if the pointer at this+0x188 equals &data_ov085_0213072c,
 * bump the angle at this+0x8e by 0x500; run func_ov085_0212d2b8(this); then
 * virtual-call slot +0x14 on the sub-object at this+0x110 with arg 0. Returns 1.
 */

typedef unsigned int u32;
typedef signed short s16;

struct SubVt {
    char _pad0[0x14];
    void (*fn14)(void* self, int a); /* 0x14 */
};

struct Sub {
    struct SubVt* vt;     /* 0x0 */
};

struct Obj {
    char _pad0[0x8e];
    s16 angle;            /* 0x8e */
    char _pad1[0x110 - 0x90];
    struct Sub sub;       /* 0x110 */
    char _pad2[0x188 - 0x114];
    void* unk188;         /* 0x188 */
};

extern u32 data_ov085_0213072c;
extern void func_ov085_0212d2b8(struct Obj* self);

int func_ov085_0212d39c(struct Obj* self)
{
    if (self->unk188 == (void*)&data_ov085_0213072c) {
        *(s16*)(((long long)(int)((char*)self + 0x8e)) & 0xFFFFFFFFFFFFFFFFLL) += 0x500;
    }
    func_ov085_0212d2b8(self);
    {
        struct Sub* s = (struct Sub*)(((long long)(int)((char*)self + 0x110)) & 0xFFFFFFFFFFFFFFFFLL);
        s->vt->fn14(s, 0);
    }
    return 1;
}
