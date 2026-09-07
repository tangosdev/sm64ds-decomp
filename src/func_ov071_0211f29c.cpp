//cpp
extern "C" void *_ZN8dActor_c7FindEggER5dCc_c(void *self, void *clsn);
extern "C" void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int id, void *pos);
extern "C" void func_ov071_0211f498(void *c);
extern "C" void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
extern "C" int _ZN6Player9IsOnShellEv(void *p);
extern "C" short Vec3_HorzAngle(void *a, void *b);
extern "C" void _ZN6Player16IncMegaKillCountEv(void *p);
extern "C" int _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(void *self, void *clsn, void *player);
extern "C" void _ZN6Player6BounceE5Fix12IiE(void *p, int fix);
extern "C" int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, void *pos, unsigned int a, int fix, unsigned int b, unsigned int cc, unsigned int d);
extern "C" int Scuttlebug_SetState(void *c, int i);

extern "C" void func_ov071_0211f29c(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    unsigned char *r4;
    int b;

    if (_ZN8dActor_c7FindEggER5dCc_c(c, c + 0x160) != 0) {
        _ZN5Sound9PlayBank0EjRK7Vector3(9, c + 0x74);
        func_ov071_0211f498(c);
        return;
    }

    {
        unsigned int id = *(unsigned int *)(c + 0x184);
        if (id == 0)
            return;
        r4 = (unsigned char *)_ZN8dActor_c10FindWithIDEj(id);
    }
    if (r4 == 0)
        return;

    b = (int)(*(unsigned short *)(r4 + 0xc) == 0xbf);
    if (b == 0)
        return;

    b = (int)((*(int *)(c + 0xb0) & 0x20000) != 0);
    if (b != 0) {
        Scuttlebug_SetState(c, 6);
        return;
    }

    if ((*(int *)(c + 0x180) & 0x66fe0)
        || _ZN6Player9IsOnShellEv(r4) != 0
        || *(unsigned char *)(r4 + 0x6f9) != 0) {
        _ZN5Sound9PlayBank0EjRK7Vector3(9, c + 0x74);
        func_ov071_0211f498(c);
        return;
    }

    if (*(int *)(c + 0x180) & 0x10) {
        *(short *)(c + 0x94) = Vec3_HorzAngle(r4 + 0x5c, c + 0x5c);
        *(short *)(c + 0x8e) = (short)(*(short *)(c + 0x94) + 0x8000);
        _ZN6Player16IncMegaKillCountEv(r4);
        Scuttlebug_SetState(c, 8);
        return;
    }

    if (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(c, c + 0x160, r4) != 0) {
        _ZN6Player6BounceE5Fix12IiE(r4, 0x28000);
        func_ov071_0211f498(c);
        return;
    }

    if (*(int *)(c + 0x39c) == 7)
        return;

    {
        int v[3];
        v[0] = *(int *)(c + 0x5c);
        v[1] = *(int *)(c + 0x60);
        v[2] = *(int *)(c + 0x64);
        if (_ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r4, v, 1, 0xc000, 1, 0, 1) != 0)
            Scuttlebug_SetState(c, 4);
    }
}
