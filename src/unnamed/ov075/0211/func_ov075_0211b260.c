extern int __aeabi_idiv(int a, int b);

struct Vtx {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
};

struct T {
    char pad0[0x80];
    struct Vtx *p80;
    char pad1[0xa6 - 0x84];
    unsigned char ba6;
    unsigned char ba7;
};

void func_ov075_0211b260(struct T *t)
{
    int q[4];
    int acc;
    int j;
    int i;
    int uu;
    int vv;
    int nb;

    acc = 0;
    q[0] = acc;
    j = 0;
    if (((int)t->ba7) <= 0)
        return;

    q[2] = acc;
    q[3] = acc;
    q[1] = acc;

    do
    {
        i = q[1];
        if (((int)t->ba6) > 0)
        {
            do
            {
                struct Vtx *e;
                int b;
                int c;

                e = &t->p80[j * t->ba7 + i];
                e->f0 = acc;
                e->f4 = q[0];
                e->f8 = q[2];
                e->f10 = 0x1ff00000;

                uu = __aeabi_idiv(0x80000, ((int)t->ba6) - 1);
                vv = __aeabi_idiv(0x80000, ((int)t->ba7) - 1);
                c = ((0x80000 - vv * j) << 8) >> 16;
                b = ((uu * i) << 8) >> 16;
                e->f14 = (unsigned short)b | ((unsigned short)c << 16);

                nb = (int)t->ba6;
                if (i == nb - 2)
                    acc = 0x1f4000;
                else
                    acc += __aeabi_idiv(0x1f4000, nb - 1);
                i++;
            } while (i < nb);
        }

        nb = (int)t->ba7;
        acc = q[3];
        if (j == nb - 2)
            q[0] = 0x1f4000;
        else
            q[0] += __aeabi_idiv(0x1f4000, nb - 1);

        j++;
    } while (j < nb);
}
