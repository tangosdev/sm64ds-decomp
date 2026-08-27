// @symbol func_ov091_021339fc
#include "common.h"
#include "decl_Enemy.h"
#include "decl_Player.h"
#include "decl_SaveData.h"

extern void *_ZN8dActor_c10FindWithIDEj(u32 id);
extern void func_ov002_020aea30(void *c, void *a, u32 unused);
extern void _ZN8dActor_c8PoofDustEv(void *a);
extern void _ZN7fBase_c18MarkForDestructionEv(void *a);
extern void _ZN6Player16IncMegaKillCountEv(void *p);
extern void func_02012694(int a, void *p);
extern int _ZN6Player15IsCollectingCapEv(void *p);
extern void _ZN6Player18SetNewHatCharacterEjjb(void *p, u32 a, u32 b, u32 c2);
extern void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 id, u32 flags, Vector3 *pos, Vector3_16 *rot, s8 a, s16 b);

void func_ov091_021339fc(char *c)
{
    char *a;
    u32 fl;
    u32 id = *(u32 *)(c + 0x134);

    if (id == 0)
        return;
    a = (char *)_ZN8dActor_c10FindWithIDEj(id);
    if (*(u32 *)(c + 0x374) == 0) {
        fl = *(u32 *)(c + 0x130);
        if ((fl & 0x40000) != 0) {
            *(u32 *)(c + 0x10c) = 4;
            func_ov002_020aea30(c, a, 0);
            return;
        }
        if ((fl & 0x2000) != 0) {
            _ZN8dActor_c8PoofDustEv(c);
            _ZN7fBase_c18MarkForDestructionEv(c);
            return;
        }
        {
            int b = (int)(*(u16 *)(a + 0xc) == 0xbf);
            if (b == 0)
                return;
        }
        if (*(u8 *)(a + 0x6f9) == 1) {
            _ZN8dActor_c8PoofDustEv(c);
            _ZN7fBase_c18MarkForDestructionEv(c);
            return;
        }
        if ((fl & 0x10) == 0)
            return;
        _ZN8dActor_c8PoofDustEv(c);
        _ZN12dEnemyBase_c22SpawnMegaCharParticlesER8dActor_cPc(c, a, 0);
        _ZN6Player16IncMegaKillCountEv(a);
        func_02012694(0x1d, c + 0x74);
        _ZN7fBase_c18MarkForDestructionEv(c);
        return;
    }

    {
        int b = (int)(*(u16 *)(a + 0xc) == 0xbf);
        if (b == 0)
            return;
    }
    if (*(u8 *)(a + 0x6f9) != 0)
        return;
    if (*(u8 *)(a + 0x703) != 0)
        return;
    if (_ZN6Player15IsCollectingCapEv(a) != 0)
        return;
    _ZN6Player8BlowAwayEs(a, *(s16 *)(c + 0x94));
    {
        u8 capFlag = *(u8 *)(a + 0x6ff);
        u8 hat = *(u8 *)(a + 0x6d9);
        if (capFlag != 0)
            return;
        if (*(u8 *)(a + 0x6fd) != 0)
            return;
        {
            u32 cur = *(u32 *)(a + 8);
            u32 param = 1;
            if (hat != cur) {
                _ZN6Player18SetNewHatCharacterEjjb(a, hat, 0, 0);
            } else {
                if (_ZN8SaveData16HasPlayerLostCapEv() != 0)
                    return;
                _ZN8SaveData13PlayerLoseCapEv();
            }
            {
                u32 curHat1 = *(u32 *)(a + 8);
                Vector3_16 rot;
                void *spawned;
                rot.x = 0;
                rot.y = 0;
                rot.z = 0;
                rot.y = *(s16 *)(c + 0x94);
                param = param | (curHat1 << 8);
                spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                    0x10d, param, (Vector3 *)(c + 0x5c), &rot,
                    *(s8 *)(c + 0xcc), -1);
                if (spawned == 0)
                    return;
                *(u32 *)((char *)spawned + 0x98) = 0x32000;
                *(u32 *)((char *)spawned + 0xa4) = 0;
                *(u32 *)((char *)spawned + 0xa8) = 0x14000;
                *(u32 *)((char *)spawned + 0xac) = 0;
            }
        }
    }
}
