#include "private/ov006_ec4dc_obj.h"

extern s32 _ZN4cstd4fdivEii(s32 numerator, s32 denominator);
extern void func_ov006_020ec2bc(Obj *self);
extern s32 data_ov006_0213c958;
extern Vec2i data_ov006_0213ca24;

void func_ov006_020ec4dc(Obj *self, int n)
{
    int b = n % 3;
    int a = n / 3;
    int rA, rB, rC, rD;
    int g1;
    int cm, dm;

    self->f40 = 0xa00;
    self->f44 = 0xa00;
    g1 = data_ov006_0213c958 / 3 - 1;

    rA = _ZN4cstd4fdivEii(a << 12, g1 << 12);
    rB = _ZN4cstd4fdivEii((g1 - a) << 12, g1 << 12);
    rC = _ZN4cstd4fdivEii(b << 12, 0x2000);
    rD = _ZN4cstd4fdivEii((2 - b) << 12, 0x2000);

    if (a & 1) {
        self->s76 = -0x4000;
        self->s84 = -0x8000;
        cm = 0x8000;
        dm = 0xfe000;
    } else {
        self->s76 = 0x4000;
        self->s84 = 0;
        cm = 0x100000;
        dm = 0xa000;
    }

    {
        Vec2i base;
        int i = 0;
        int delta;

        base.x = 0;
        base.y = 0;
        delta = 0;
        base.x = (int)(((long long)cm * rC + 0x800) >> 12) + (int)(((long long)dm * rD + 0x800) >> 12);
        base.y = (int)((((long long)rA << 16) + 0x800) >> 12) + (int)(((long long)rB * 0xb0000 + 0x800) >> 12);

        for (; i < 5; i++) {
            int bx = *(volatile int *)&base.x;
            self->arr[i].x = bx;
            self->arr[i].y = *(volatile int *)&base.y;
            if (self->s76 < 0)
                self->arr[i].x += delta;
            else
                self->arr[i].x -= delta;
            self->s7a[i] = self->s76;
            delta += 0x20000;
        }
    }

    func_ov006_020ec2bc(self);
    self->f68 = 0;
    self->f6c = 0x1ec;
    self->v0 = data_ov006_0213ca24;
    self->v1 = self->v0;
}
