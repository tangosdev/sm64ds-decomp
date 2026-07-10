extern int func_0201267c(int, void *, int);
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, unsigned);
extern void *_ZN5Actor13ClosestPlayerEv(void *);
extern short Vec3_VertAngle(const void *, const void *);
extern int data_ov014_02114970[];
extern short data_02082214[];

void func_ov014_02111b70(char *c)
{
    int tgt[3];
    char *p;
    int a;
    int i;
    int v;

    *((int *)(c + 0x9c)) = 0;
    func_0201267c(0x3a, c + 0x74, 0);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x150, (void *)data_ov014_02114970[1], 0, 0x1000, 0);
    p = (char *)_ZN5Actor13ClosestPlayerEv(c);

    /* ROM load order: y, z, x — then y+0x50000, store x, setup call, store y/z */
    {
        int y = *((int *)(p + 0x60));
        int z = *((int *)(p + 0x64));
        int x = *((int *)(p + 0x5c));
        tgt[0] = x;
        tgt[1] = y + 0x50000;
        tgt[2] = z;
    }

    a = (unsigned short)Vec3_VertAngle(c + 0x5c, tgt);
    i = (a >> 4) << 1;
    *((int *)(c + 0xa8)) = -((int)(((((long long)data_02082214[i]) * 0x8c000) + 0x800) >> 12));
    v = *((int *)(c + 0xa8));
    if (v < 0x5000) {
        v = 0x5000;
    } else if (v > 0x2d000) {
        v = 0x2d000;
    }
    *((int *)(c + 0xa8)) = v;
    *((int *)(c + 0x98)) = (int)(((((long long)data_02082214[i + 1]) * 0x8c000) + 0x800) >> 12);
    *((short *)(c + 0x5fc)) = 0x3c;
}
