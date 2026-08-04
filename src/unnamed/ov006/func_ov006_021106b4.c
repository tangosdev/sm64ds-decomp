typedef int Fix12i;

struct Mgr {
    char pad0[0x4668];
    int count;                  /* 0x4668 */
    char pad1[0x4688 - 0x466c];
    int *arr[13];               /* 0x4688 */
    char pad2[0x4780 - (0x4688 + 13 * 4)];
    void *field4780;            /* 0x4780 */
};

struct Obj {
    char pad0[4];
    struct Mgr *mgr;            /* 0x4 */
    int x;                      /* 0x8 */
    int y;                      /* 0xc */
    int f10;                    /* 0x10 */
    int f14;                    /* 0x14 */
    char pad1[0x30 - 0x18];
    unsigned char b30;          /* 0x30 */
    unsigned char b31;          /* 0x31 */
    char pad2[0x34 - 0x32];
    int f34;                    /* 0x34 */
    int f38;                    /* 0x38 */
};

extern void func_ov006_0211470c(int *a, int *b);
extern void Vec2_Sub(int *o, int *a, int *b);
extern Fix12i Vec2_Len(const void *v);
extern void func_ov006_0210d8bc(char *c);
extern void func_02012718(void *a, int b);

void func_ov006_021106b4(struct Obj *self)
{
    int s[5];
    int i;
    int b;

    if (self->b30 == 0) return;
    self->f10 = self->x;
    self->f14 = self->y;
    if (self->f38 > 0) {
        *(int *)(((int)self + 0x38)) -= 1;
        if (self->f38 == 0) self->b30 = 0;
        return;
    }
    for (i = 0; i < self->mgr->count; i++) {
        func_ov006_0211470c(&s[0], (i >= 13) ? 0 : self->mgr->arr[i]);
        Vec2_Sub(&s[2], &self->x, &s[0]);
        b = Vec2_Len(&s[2]) < 0x4000;
        if (b) {
            self->f38 = 0x61;
            self->b31 = 0;
            func_ov006_0210d8bc((char *)self->mgr->field4780);
            {
                struct Mgr *m2 = self->mgr;
                *((char *)((i >= 13) ? 0 : m2->arr[i]) + 0x30) = 0;
            }
            func_02012718((void *)0x1a0, self->x);
            return;
        }
    }
    *(int *)(((int)self + 0x34)) += 1;
    if (self->f34 < 0x3c) return;
    self->f34 = 0;
    if (self->b31 == 0) self->b31 = 1;
    else self->b31 = 0;
}
