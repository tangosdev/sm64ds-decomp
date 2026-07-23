struct V3 { int x, y, z; };
struct S { char _0[8]; unsigned short f8; char _a[0x1a]; int *f24; int *f28; };
extern int func_ov007_020c2e90(struct S *s, struct V3 *b, struct V3 *c, int d, int e);
extern void func_ov007_020c2d44(struct S *s, int i);
extern int func_ov007_020c2dfc(struct S *self, int idx, int v);
extern void func_ov007_020c24d0(struct S *s, int i);
int func_ov007_020c25fc(struct S *self, int a1, int a2, int a3, int e, int v)
{
    int ok = 1;
    if (self->f8 != 0) {
        struct V3 loc1, loc2;
        loc1.x = a1; loc1.y = a2;
        loc2.x = self->f24[self->f8 - 1];
        loc2.y = self->f28[self->f8 - 1];
        loc2.z = 0; loc1.z = 0;
        ok = func_ov007_020c2e90(self, &loc1, &loc2, a3, e);
    }
    if (ok != 0) {
        self->f24[self->f8] = a1;
        self->f28[self->f8] = a2;
        func_ov007_020c2d44(self, self->f8);
        {
            unsigned short *p = (unsigned short *)(((int)self + 8) & 0xFFFFFFFFFFFFFFFFLL);
            *p = *p + 1;
        }
        if (func_ov007_020c2dfc(self, self->f8 - 1, v) != 0)
            func_ov007_020c24d0(self, self->f8 - 2);
        return 1;
    }
    return 0;
}
