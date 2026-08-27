#pragma opt_lifetimes off
typedef struct Ent
{
    int x;
    unsigned char pad0[0xe];
    unsigned char f12;
    unsigned char f13;
    unsigned char f14;
    unsigned char pad1;
    unsigned char f16;
    unsigned char pad2;
} Ent;

typedef struct Big
{
    unsigned char pad0[0x51a8];
    Ent arr[12];
    unsigned char pad1[0x74];
    unsigned char mode;
} Big;

void func_ov006_020f3f84(Big *p, int i)
{
    int j;
    int n;
    unsigned char *pf;
    int *px;

    px = &p->arr[i].x;
    pf = &p->arr[i].f16;
    *px -= 0x10000;
    if (*pf == 0)
    {
        if (p->mode == 1)
        {
            if (i == 6 || i == 0xb)
            {
                *pf = 1;
            }
            n = (i >= 7) ? 0xc - i : 7 - i;
            for (j = 1; j < n; j++)
            {
                if (p->arr[i + j].f13 != 0)
                {
                    if ((((*(int *)((char *)(&((Ent *)((void *)p))[i]) + 0x51a8)) - p->arr[i + j].x) >> 12) <= 4)
                    {
                        *pf = 1;
                        p->arr[i + j].f14 = 6;
                        break;
                    }
                }
            }
        }
        else
        {
            if ((i & 3) == 3)
            {
                *pf = 1;
            }
            j = 4 - (i & 3);
            for (n = 1; n < j; n++)
            {
                if (p->arr[i + n].f13 != 0)
                {
                    if ((((*(int *)((char *)(&((Ent *)((void *)p))[i]) + 0x51a8)) - p->arr[i + n].x) >> 12) <= 4)
                    {
                        *pf = 1;
                        p->arr[i + n].f14 = 6;
                        break;
                    }
                }
            }
        }
    }
    if ((*px >> 12) > -0x18)
    {
        return;
    }
    p->arr[i].f13 = 0;
    p->arr[i].f12 = 0;
}
