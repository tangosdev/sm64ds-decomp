typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct Vec3 { int x, y, z; };

extern void *_ZN5Actor10FindWithIDEj(u32 id);
extern void func_020aea30(void *c, void *a, u32 unused);
extern void _ZN5Actor8PoofDustEv(void *a);
extern void _ZN9ActorBase18MarkForDestructionEv(void *a);
extern void _ZN5Enemy22SpawnMegaCharParticlesER5ActorPc(void *self, void *a, char *nullArg);
extern void _ZN6Player16IncMegaKillCountEv(void *p);
extern void func_02012694(int a, void *p);
extern int _ZN6Player15IsCollectingCapEv(void *p);
extern void _ZN6Player8BlowAwayEs(void *p, short a);
extern void _ZN6Player18SetNewHatCharacterEjjb(void *p, u32 a, u32 b, u32 c2);
extern int _ZN8SaveData16HasPlayerLostCapEv(void);
extern void _ZN8SaveData13PlayerLoseCapEv(void);
extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 id, u32 flags, struct Vec3 *pos, short *rot, int a, int b);

void func_ov091_021339fc(char *c)
{
    char *a;
    u32 fl;
    u32 id = *(u32*)(c + 0x134);

    if (id == 0) return;
    a = (char*)_ZN5Actor10FindWithIDEj(id);
    if (*(u32*)(c + 0x374) == 0) {
        fl = *(u32*)(c + 0x130);
        if ((fl & 0x40000) != 0) {
            *(u32*)(c + 0x10c) = 4;
            func_020aea30(c, a, 0);
            return;
        }
        if ((fl & 0x2000) != 0) {
            _ZN5Actor8PoofDustEv(c);
            _ZN9ActorBase18MarkForDestructionEv(c);
            return;
        }
        {
            int b = (int)(*(u16*)(a + 0xc) == 0xbf);
            if (b == 0) return;
        }
        if (*(u8*)(a + 0x6f9) == 1) {
            _ZN5Actor8PoofDustEv(c);
            _ZN9ActorBase18MarkForDestructionEv(c);
            return;
        }
        if ((fl & 0x10) == 0) return;
        _ZN5Actor8PoofDustEv(c);
        _ZN5Enemy22SpawnMegaCharParticlesER5ActorPc(c, a, 0);
        _ZN6Player16IncMegaKillCountEv(a);
        func_02012694(0x1d, c + 0x74);
        _ZN9ActorBase18MarkForDestructionEv(c);
        return;
    }

    {
        int b = (int)(*(u16*)(a + 0xc) == 0xbf);
        if (b == 0) return;
    }
    if (*(u8*)(a + 0x6f9) != 0) return;
    if (*(u8*)(a + 0x703) != 0) return;
    if (_ZN6Player15IsCollectingCapEv(a) != 0) return;
    _ZN6Player8BlowAwayEs(a, *(short*)(c + 0x94));
    {
        u8 hat = *(u8*)(a + 0x6d9);
        if (*(u8*)(a + 0x6ff) != 0) return;
        if (*(u8*)(a + 0x6fd) != 0) return;
        {
        u32 newHat, curHat0;
        asm {
            ldr curHat0, [a, #8]
            mov newHat, #1
        }
        if (hat != curHat0) {
            _ZN6Player18SetNewHatCharacterEjjb(a, hat, 0, 0);
        } else {
            if (_ZN8SaveData16HasPlayerLostCapEv() != 0) return;
            _ZN8SaveData13PlayerLoseCapEv();
        }
        {
            u32 curHat1 = *(u32*)(a + 8);
            short rot[3];
            void *spawned;
            rot[0] = 0;
            rot[1] = 0;
            rot[2] = 0;
            rot[1] = *(short*)(c + 0x94);
            spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                0x10d, newHat | (curHat1 << 8), (struct Vec3*)(c + 0x5c), rot, *(signed char*)(c + 0xcc), -1);
            if (spawned == 0) return;
            *(u32*)((char*)spawned + 0x98) = 0x32000;
            *(u32*)((char*)spawned + 0xa4) = 0;
            *(u32*)((char*)spawned + 0xa8) = 0x14000;
            *(u32*)((char*)spawned + 0xac) = 0;
        }
        }
    }
}
