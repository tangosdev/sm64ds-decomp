/* func_ov074_021207b8 — if func_ov074_02121a20(this, 6) and the animation at
 * this+0x260 has finished, flip the angle at this+0x8e by 0x8000 and return 1;
 * else return 0.
 * Callees: func_ov074_02121a20, _ZN9Animation8FinishedEv.
 */

typedef signed short s16;

struct Anim {
    char _pad0[4];
};

struct Obj {
    char _pad0[0x8e];
    s16 angle;            /* 0x8e */
    char _pad1[0x260 - 0x90];
    struct Anim anim;     /* 0x260 */
};

extern int func_ov074_02121a20(struct Obj* self, int a);
extern int _ZN9Animation8FinishedEv(struct Anim* anim);

int func_ov074_021207b8(struct Obj* self)
{
    if (func_ov074_02121a20(self, 6)) {
        if (_ZN9Animation8FinishedEv(&self->anim)) {
            *(s16*)(((long long)(int)((char*)self + 0x8e)) & 0xFFFFFFFFFFFFFFFFLL) += 0x8000;
            return 1;
        }
    }
    return 0;
}
