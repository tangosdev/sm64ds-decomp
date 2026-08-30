//cpp
#include "daOts_c.h"

struct BCA_File;

extern "C" {
extern void* _ZN8dActor_c10FindWithIDEj(u32 id);
extern void func_ov064_02115f98(void* c, void* p);
extern int _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(void* c, void* v, void* r4, s32 flag);
extern int _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(void* c, void* clsn, void* player);
extern int _ZN6Player9IsOnShellEv(void* p);
extern int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, const Vector3* v, u32 a, s32 f, u32 b, u32 c, u32 d);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, BCA_File* f, int i, s32 fx, u32 j);
}

extern "C" void func_ov064_02116754(char* c)
{
    void* r4;
    s32 r1;
    u32 id;

    if (*(s32*)(c + 0x398) == 2)
        return;
    id = *(u32*)(c + 0x360);
    if (id == 0)
        return;

    r4 = _ZN8dActor_c10FindWithIDEj(id);
    if (!r4)
        return;

    {
        int isBf = (int)(*(u16*)((char*)r4 + 0xc) == 0xbf);
        if (!isBf)
            return;
    }

    r1 = *(s32*)(c + 0x35c);
    if ((r1 & 0x7c0) || *(u8*)((char*)r4 + 0x6f9) != 0) {
        *(s16*)(c + 0x94) = *(s16*)((char*)r4 + 0x8e);
        if (*(s32*)((char*)r4 + 8) == 2)
            *(s32*)(c + 0x98) = 0x32000;
        else
            *(s32*)(c + 0x98) = 0x28000;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, *(BCA_File**)(*(char**)(*(char**)(c + 0x330) + 8) + 4), 0, 0x1000, 0);
        func_ov064_02115f98(c, r4);
        *(s32*)(c + 0x398) = 2;
        *(u8*)(c + 0x3f9) = 0;
        ((daOts_c*)c)->PlayHitSound();
        return;
    }

    if (r1 & 0x2000) {
        *(s16*)(c + 0x94) = *(s16*)((char*)r4 + 0x94);
        *(s32*)(c + 0x98) = 0x28000;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, *(BCA_File**)(*(char**)(*(char**)(c + 0x330) + 8) + 4), 0, 0x1000, 0);
        func_ov064_02115f98(c, r4);
        *(s32*)(c + 0x398) = 2;
        *(u8*)(c + 0x3f9) = 0;
        ((daOts_c*)c)->PlayHitSound();
        return;
    }

    {
        int isD8 = (int)(*(u16*)(c + 0xc) == 0xd8);
        if (!isD8 && (r1 & 0x10)) {
            s16 v[3];
            v[0] = 0x2000;
            v[1] = 0;
            v[2] = 0;
            _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(c, v, r4, *(s32*)(*(char**)(c + 0x330) + 0x24));
            ((daOts_c*)c)->PlayHitSound();
            return;
        }
    }

    if (r1 & 0x40000) {
        *(s16*)(c + 0x94) = *(s16*)((char*)r4 + 0x8e);
        *(s32*)(c + 0x98) = 0x39800;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, *(BCA_File**)(*(char**)(*(char**)(c + 0x330) + 8) + 4), 0, 0x1000, 0);
        func_ov064_02115f98(c, r4);
        *(s32*)(c + 0x398) = 2;
        *(u8*)(c + 0x3f9) = 0;
        ((daOts_c*)c)->PlayHitSound();
        return;
    }

    if (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(c, c + 0x33c, r4) != 0) {
        *(s16*)(c + 0x94) = *(s16*)((char*)r4 + 0x8e);
        if (*(s32*)((char*)r4 + 8) == 2)
            *(s32*)(c + 0x98) = 0x32000;
        else
            *(s32*)(c + 0x98) = 0x28000;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, *(BCA_File**)(*(char**)(*(char**)(c + 0x330) + 8) + 4), 0, 0x1000, 0);
        func_ov064_02115f98(c, r4);
        *(s32*)(c + 0x398) = 2;
        *(u8*)(c + 0x3f9) = 0;
        ((daOts_c*)c)->PlayHitSound();
        return;
    }

    if (_ZN6Player9IsOnShellEv(r4) != 0) {
        *(s32*)((char*)r4 + 0x98) = -*(s32*)(c + 0x98);
        *(s16*)(c + 0x94) = (s16)(*(s16*)(c + 0x8e) + 0x8000);
        *(s32*)(c + 0x98) = 0x28000;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, *(BCA_File**)(*(char**)(*(char**)(c + 0x330) + 0x10) + 4), 0, 0x1000, 0);
        ((daOts_c*)c)->PlayShellHitSound();
        func_ov064_02115f98(c, r4);
        *(s32*)(c + 0x398) = 2;
        *(u8*)(c + 0x3f9) = 0;
        ((daOts_c*)c)->PlayHitSound();
        return;
    }

    if (*(u8*)((char*)r4 + 0x6fb) != 0)
        return;

    *(s32*)(c + 0x398) = 2;
    *(u8*)(c + 0x3f9) = 0;
    ((daOts_c*)c)->PlayHitSound();

    {
        Vector3 v;
        v.x = *(s32*)(c + 0x5c);
        v.y = *(s32*)(c + 0x60);
        v.z = *(s32*)(c + 0x64);
        if (_ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r4, &v, 0, 0x14000, 1, 0, 1) != 0) {
            func_ov064_02115f98(c, r4);
        }
    }

    *(s16*)(c + 0x94) = (s16)(*(s16*)(c + 0x8e) + 0x8000);
    {
        int isD8 = (int)(*(u16*)(c + 0xc) == 0xd8);
        if (!isD8)
            *(s32*)(c + 0x98) = 0x14000;
        else
            *(s32*)(c + 0x98) = 0xa000;
    }
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, *(BCA_File**)(*(char**)(*(char**)(c + 0x330) + 0x10) + 4), 0, 0x1000, 0);
    ((daOts_c*)c)->PlayShellHitSound();
}
