extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern int ApproachAngle(short *p, int target, int a, int b, int c);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void *pos);
extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, void *pos, void *v16, int e, int f);
extern void func_020393a4(void *p, int v);
extern void func_02039394(void *p, int v);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *self);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *self, int fix, int t);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);

int func_ov022_021112ac(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    short *ip;

    switch (*(unsigned char *)(c + 0x31e)) {
    case 0:
        if (DecIfAbove0_Short((unsigned short *)(c + 0x322)) != 0)
            break;
        if (*(unsigned char *)(c + 0x320) == 0)
            break;
        if (ApproachAngle((short *)(c + 0x96), 0, 0xf, 0x30, 2) != 0)
            break;
        *(unsigned short *)(c + 0x300 + 0x22) = 0x96;
        *(unsigned char *)(c + 0x31e) = 1;
        _ZN5Sound9PlayBank3EjRK7Vector3(0xc, c + 0x74);
        break;
    case 1:
        if (DecIfAbove0_Short((unsigned short *)(c + 0x322)) == 0) {
            if (ApproachAngle((short *)(c + 0x96), -0x100, 0xf, 0x30, 2) != 0)
                break;
            *(unsigned short *)(c + 0x300 + 0x22) = 0x96;
            *(unsigned char *)(c + 0x31e) = 0;
            break;
        }
        {
            int b8;
            b8 = (int)((*(int *)(c + 0xb0) & 8) != 0);
            if (b8 != 0)
                break;
        }
        if (DecIfAbove0_Byte((unsigned char *)(c + 0x31f)) != 0)
            break;
        {
            int v[3];
            void *s;
            v[0] = *(int *)(c + 0x5c);
            v[1] = *(int *)(c + 0x60);
            v[2] = *(int *)(c + 0x64);
            v[1] = *(int *)(c + 0x60) + 0x1f4000;
            s = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xf3, 0, v, 0, *(signed char *)(c + 0xcc), -1);
            *(void **)((unsigned char *)s + 0x10c) = c;
            *(int *)((unsigned char *)s + 0x118) = *(int *)(c + 0x60);
            {
                unsigned short *t = (unsigned short *)((long long)(int)(c + 0x324) & 0xFFFFFFFFFFFFFFFFLL);
                *t = *t + 1;
            }
            *(unsigned char *)(c + 0x31f) = 2;
        }
        break;
    }

    ip = (short *)((long long)(int)(c + 0x94) & 0xFFFFFFFFFFFFFFFFLL);
    *ip = (short)(*ip + *(short *)(c + 0x96));
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
    func_020393a4(c + 0x124, 0x780000);
    func_02039394(c + 0x124, 0x1000);
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0x780000, 0x1000) != 0)
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    *(unsigned char *)(c + 0x320) = 0;
    return 1;
}
