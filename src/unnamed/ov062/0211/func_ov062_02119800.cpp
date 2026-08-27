//cpp
extern "C" void func_0201267c(unsigned int id, void *p);
extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern short data_02082214[];

extern "C" void func_ov062_02119800(char *c)
{
    volatile int stack[3];
    char *self = c;
    unsigned int kind = ((unsigned int)(*(int *)(self + 0x358) << 4)) >> 16;
    short ang;
    int x, y, z;

    if (kind < 2)
        goto check_hi;
    if (kind <= 8)
        goto body;
check_hi:
    if (kind < 0x13)
        goto reset;
    if (kind > 0x19)
        goto reset;

body:
    if (*(unsigned char *)(self + 0x3b2) != 0)
        return;
    func_0201267c(0xe4, self + 0x74);
    *(unsigned char *)(self + 0x3b2) = 1;

    ang = *(short *)(self + 0x8e);
    x = *(int *)(self + 0x5c);
    /* Use kind right after x so cmp lands after both loads of ang and x */
    stack[0] = (kind > 8) ? x : x;
    y = *(int *)(self + 0x60);
    ang = (short)(ang + 0x4000);
    stack[1] = y;
    z = *(int *)(self + 0x64);
    stack[2] = z;
    stack[1] = y + 0x1e000;

    if (kind > 8)
        goto add_path;

    {
        unsigned short u = (unsigned short)ang;
        int n = (int)(u >> 4);
        short cs = data_02082214[n * 2];
        short sn = data_02082214[n * 2 + 1];
        short k = 0x1e;
        stack[0] = x - (int)(cs * k);
        stack[2] = z - (int)(sn * k);
    }
    goto do_new;

add_path:
    {
        unsigned short u = (unsigned short)ang;
        int n = (int)(u >> 4);
        short cs = data_02082214[n * 2];
        short sn = data_02082214[n * 2 + 1];
        short k = 0x1e;
        stack[0] = (int)(cs * k) + x;
        stack[2] = (int)(sn * k) + z;
    }
do_new:
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xf9, (int)stack[0], (int)stack[1], (int)stack[2]);
    return;

reset:
    *(unsigned char *)(self + 0x3b2) = 0;
}
