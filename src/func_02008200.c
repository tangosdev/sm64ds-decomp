typedef unsigned short u16;

extern struct Actor *func_0200e6d8(unsigned int arg0);
extern void Math_Function_0203b0fc(int *p, int target, int scale, int max);

extern u16 KS_FRAME_COUNTER;

typedef struct Actor {
    char pad60[0x60];
    int f60;            /* 0x60 */
    char pad64[0x644 - 0x64];
    int f644;           /* 0x644 */
} Actor;

typedef struct Self {
    char pad84[0x84];
    int f84;            /* 0x84 */
    char pad88[0xb4 - 0x88];
    int fb4;            /* 0xb4 */
} Self;

int func_02008200(Self *self, int dummy, int arg2)
{
    Actor *a;
    int diff, val, target;
    if (arg2 == KS_FRAME_COUNTER)
        self->fb4 = 0x190000;
    a = func_0200e6d8(0);
    diff = a->f60 - a->f644;
    val = (int)(((long long)diff * 0x1199 + 0x800) >> 12);
    target = a->f644 + 0x50000 + val;
    Math_Function_0203b0fc(&self->fb4, target, 0xa3, 0x7fffffff);
    if (self->f84 < self->fb4)
        self->f84 = self->fb4;
    return 1;
}
