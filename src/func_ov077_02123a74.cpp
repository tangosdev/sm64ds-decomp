//cpp
extern "C" void *_ZN5Actor7FindEggER12CylinderClsn(void *self, void *clsn);
extern "C" void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int id, void *pos);
extern "C" void func_ov077_02123a1c(void *c);
extern "C" void *_ZN5Actor10FindWithIDEj(unsigned int id);
extern "C" int _ZN5Actor24BumpedUnderneathByPlayerER6Player(void *self, void *player);
extern "C" int _ZN6Player9IsOnShellEv(void *p);
extern "C" short Vec3_HorzAngle(void *a, void *b);
extern "C" void _ZN6Player16IncMegaKillCountEv(void *p);
extern "C" int _ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(void *self, void *clsn, void *player);
extern "C" void _ZN6Player6BounceE5Fix12IiE(void *p, int fix);
extern "C" void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, void *pos, unsigned int a, int fix, unsigned int b, unsigned int cc, unsigned int d);
extern "C" int func_ov077_0212478c(void *c, int i);

extern "C" void func_ov077_02123a74(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    unsigned char *r4;
    int b;

    if (_ZN5Actor7FindEggER12CylinderClsn(c, c + 0x1c4) != 0) {
        _ZN5Sound9PlayBank0EjRK7Vector3(9, c + 0x74);
        func_ov077_02123a1c(c);
        return;
    }

    {
        unsigned int id = *(unsigned int *)(c + 0x1e8);
        if (id == 0)
            return;
        r4 = (unsigned char *)_ZN5Actor10FindWithIDEj(id);
    }
    if (r4 == 0)
        return;

    b = (int)(*(unsigned short *)(r4 + 0xc) == 0xbf);
    if (b == 0)
        return;

    b = (int)((*(int *)(c + 0xb0) & 0x20000) != 0);
    if (b != 0) {
        func_ov077_0212478c(c, 2);
        return;
    }

    if ((*(int *)(c + 0x1e4) & 0x66fe0)
        || _ZN5Actor24BumpedUnderneathByPlayerER6Player(c, r4) != 0
        || _ZN6Player9IsOnShellEv(r4) != 0
        || *(unsigned char *)(r4 + 0x6f9) != 0) {
        _ZN5Sound9PlayBank0EjRK7Vector3(9, c + 0x74);
        func_ov077_02123a1c(c);
        return;
    }

    if (*(int *)(c + 0x1e4) & 0x10) {
        *(short *)(c + 0x94) = Vec3_HorzAngle(r4 + 0x5c, c + 0x5c);
        *(short *)(c + 0x8e) = (short)(*(short *)(c + 0x94) + 0x8000);
        _ZN6Player16IncMegaKillCountEv(r4);
        func_ov077_0212478c(c, 4);
        return;
    }

    if (_ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(c, c + 0x1c4, r4) != 0) {
        _ZN6Player6BounceE5Fix12IiE(r4, 0x28000);
        func_ov077_02123a1c(c);
        return;
    }

    if (*(int *)(c + 0x3f4) == 3)
        return;

    {
        int v[3];
        v[0] = *(int *)(c + 0x5c);
        v[1] = *(int *)(c + 0x60);
        v[2] = *(int *)(c + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r4, v, 2, 0xc000, 1, 0, 1);
    }
}
