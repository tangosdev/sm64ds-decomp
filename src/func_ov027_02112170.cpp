//cpp
extern "C" {
extern void *_ZN5Actor10FindWithIDEj(unsigned id);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned a, int x, int y, int z);
extern void _ZN6Player8BlowAwayEs(void *o, short s);
extern int _ZN6Player15IsCollectingCapEv(void *o);
extern int Vec3_HorzAngle(void *a, void *b);
extern void _ZN6Player18SetNewHatCharacterEjjb(void *o, unsigned a, unsigned b, int c);
extern int _ZN8SaveData16HasPlayerLostCapEv(void);
extern void _ZN8SaveData13PlayerLoseCapEv(void);
extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned id, unsigned fl, void *v, void *v16, int a, int b);
extern char data_ov027_02113d10;

void func_ov027_02112170(void *cc)
{
    char *c = (char*)cc;
    char *e;
    int id;
    unsigned h;
    int ang;
    short vec16[3];
    int v[3];
    char *s;

    id = *(int*)(c + 0x24);
    if (id == 0)
        return;
    e = (char*)_ZN5Actor10FindWithIDEj(id);
    if (e == 0)
        return;

    h = *(unsigned short*)(e + 0xc);
    if ((int)(h == 0xbf) == 0)
        return;

    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x100, *(int*)(c + 0x40), *(int*)(c + 0x44), *(int*)(c + 0x48));

    if (*(unsigned char*)(e + 0x6f9))
        return;
    if (*(unsigned char*)(e + 0x703))
        return;
    if (*(unsigned char*)(e + 0x6fd))
        return;

    _ZN6Player8BlowAwayEs(e, *(short*)(c + 0x5a));

    if (_ZN6Player15IsCollectingCapEv(e))
        return;
    if (*(unsigned char*)(e + 0x6ff))
        return;
    if (*(unsigned char*)(e + 0x6fb))
        return;

    ang = Vec3_HorzAngle(&data_ov027_02113d10, e + 0x5c);
    vec16[0] = 0;
    vec16[1] = (short)ang;
    vec16[2] = 0;

    if (*(unsigned char*)(e + 0x6d9) != *(int*)(e + 8)) {
        _ZN6Player18SetNewHatCharacterEjjb(e, *(unsigned char*)(e + 0x6d9), 0, 0);
    } else {
        if (_ZN8SaveData16HasPlayerLostCapEv())
            return;
        _ZN8SaveData13PlayerLoseCapEv();
    }

    {
        int t40 = *(int*)(c + 0x40);
        int t48 = *(int*)(c + 0x48);
        int t44 = *(int*)(c + 0x44) + 0x96000;
        v[0] = t40;
        v[1] = t44;
        v[2] = t48;
    }
    s = (char*)_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
        0x10d, (*(int*)(e + 8) << 8) | 3, v, vec16, 0, -1);
    if (s == 0)
        return;

    *(int*)(s + 0xa4) = 0;
    *(int*)(s + 0xa8) = 0x14000;
    *(int*)(s + 0xac) = 0;
    *(int*)(s + 0x98) = 0x1c000;
}
}
