// NONMATCHING: register allocation (f in r0 not ip; dual ldrsh / smulbb operand coloring) (div=22)
int func_ov020_02113148(int a, short *b, short *c, int d, short e, int f, short g)
{
    int ip;
    short lr, r0, r5;
    ip = f;
    lr = b[0];
    r0 = c[0];
    b[0] = (short)(lr + r0);
    r5 = b[0];
    if (r5 != d) {
        if ((r5 - d) * (lr - d) >= 0)
            goto cont;
        {
            short lim = e;
            short vel = c[0];
            if (vel <= -lim || vel >= lim)
                goto cont;
        }
    }
    b[0] = (short)d;
    c[0] = 0;
    return 1;
cont:
    if (r5 >= d)
        ip = (short)(-ip);
    {
        short s0 = c[0];
        short s1 = c[0];
        int v = (int)s0 * (int)(short)ip;
        if (v < 0)
            ip = (short)ip * (short)g;
        c[0] = (short)(s1 + ip);
    }
    return 0;
}
