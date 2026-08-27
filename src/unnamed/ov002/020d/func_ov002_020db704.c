#include "types.h"
extern int _Z14ApproachLinearRiii(int *p, int value, int speed);

extern int data_ov002_020ff230[];
extern int data_ov002_020ff2c0[];

typedef struct Obj {
    u8 pad0[0x80];
    int x80;
    int x84;
    int x88;
    u8 pad1[0x6b2 - 0x8c];
    u16 timer;
    u8 pad2[0x6f2 - 0x6b4];
    u8 state;
    u8 phase;
    u8 pad3[0x703 - 0x6f4];
    u8 b703;
} Obj;

void func_ov002_020db704(Obj *self)
{
    int s;
    u8 i;
    int j;

    s = self->state;
    if (s == 0)
        return;

    i = 0;
    j = 0;
    switch (self->phase) {
    case 1:
        i = 2;
        j = 6;
    case 0:
        if (self->timer == 0) {
            if (s == 1)
                i++;
            if (_Z14ApproachLinearRiii(&self->x84, data_ov002_020ff2c0[s - 1 + j], data_ov002_020ff230[i])) {
                if (self->state == 1 || self->state == 6) {
                    self->timer = 10;
                } else {
                    (*(u8 *)((int)self + 0x6f2))++;
                }
            }
        } else if (self->timer == 1) {
            if (s == 1) {
                (*(u8 *)((int)self + 0x6f2))++;
            } else {
                self->state = 0;
                self->timer = 0;
                if (self->b703) {
                    self->x80 = 0x3000;
                    self->x84 = 0x3000;
                    self->x88 = 0x3000;
                    return;
                }
            }
        }
        self->x88 = (0x1000 - self->x84) / 2 + 0x1000;
        self->x80 = self->x88;
        return;
    case 2:
        if (_Z14ApproachLinearRiii(&self->x84, 0x3000, 0x200)) {
            self->state = 0;
        }
        self->x88 = self->x84;
        self->x80 = self->x88;
        return;
    case 3:
        if (_Z14ApproachLinearRiii(&self->x84, 0x1000, 0x200)) {
            self->state = 0;
        }
        self->x88 = self->x84;
        self->x80 = self->x88;
        return;
    }
}
