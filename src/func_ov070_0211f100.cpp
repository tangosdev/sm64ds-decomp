//cpp
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef struct { s32 x, y, z; } Vector3;
typedef s32 Fix12;
struct BCA_File;

extern char data_ov070_021235bc;
extern char data_ov070_021235cc;
extern u32 data_ov070_02123528[];

extern "C" {
extern void* _ZN5Actor10FindWithIDEj(u32 id);
extern void func_ov070_02120020(void* c, void* p);
extern int func_020aea30(void* c, void* p, int a, int b);
extern int _ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(void* c, void* clsn, void* player);
extern void _ZN6Player10SpinBounceE5Fix12IiE(void* p, Fix12 f);
extern void _ZN5Enemy22SpawnMegaCharParticlesER5ActorPc(void* enemy, void* actor, char* s);
extern void _ZN6Player16IncMegaKillCountEv(void* p);
extern void func_02012694(int a, void* b);
extern int _ZN6Player9IsOnShellEv(void* p);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, const Vector3* v, u32 a, Fix12 f, u32 b, u32 c, u32 d);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, BCA_File* f, int i, Fix12 fx, u32 j);
}

extern "C" void func_ov070_0211f100(char* c)
{
    void* r5;
    s32 r4;

    if (*(s32*)(c + 0x134) == 0)
        return;
    r5 = _ZN5Actor10FindWithIDEj(*(s32*)(c + 0x134));
    if (!r5)
        return;

    r4 = *(s32*)(c + 0x130);
    if (r4 & 0x40000) {
        *(s32*)(c + 0x3d8) = 1;
        func_ov070_02120020(c, &data_ov070_021235bc);
        return;
    }
    if (r4 & 0x20) {
        *(s32*)(c + 0x10c) = 1;
        func_020aea30(c, r5, 0, 1);
        return;
    }
    if (r4 & 0x67c0) {
        *(s32*)(c + 0x3d8) = 0;
        func_ov070_02120020(c, &data_ov070_021235bc);
        return;
    }

    {
        int isBf = (int)(*(u16*)((char*)r5 + 0xc) == 0xbf);
        if (!isBf)
            return;
    }
    if (*(u8*)((char*)r5 + 0x6fb) != 0)
        return;

    if (_ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(c, c + 0x110, r5)) {
        _ZN6Player10SpinBounceE5Fix12IiE(r5, 0x28000);
        *(s32*)(c + 0x10c) = 1;
        func_020aea30(c, r5, 0, 1);
        return;
    }

    if (r4 & 0x10) {
        _ZN5Enemy22SpawnMegaCharParticlesER5ActorPc(c, r5, (char*)0);
        _ZN6Player16IncMegaKillCountEv(r5);
        func_02012694(0x1d, c + 0x74);
        func_ov070_02120020(c, &data_ov070_021235bc);
        return;
    }

    if (*(u8*)((char*)r5 + 0x6f9) == 1 || _ZN6Player9IsOnShellEv(r5) == 1) {
        *(s32*)(c + 0x3d8) = 0;
        func_ov070_02120020(c, &data_ov070_021235bc);
        return;
    }

    {
        Vector3 v;
        v.x = *(s32*)(c + 0x5c);
        v.y = *(s32*)(c + 0x60);
        v.z = *(s32*)(c + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r5, &v, 2, 0xc000, 1, 0, 1);
    }
    if (*(s32*)(c + 0x3dc) != 0)
        return;
    if (*(s32*)(c + 0x3bc) != (s32)&data_ov070_021235cc)
        return;

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, (BCA_File*)data_ov070_02123528[1], 0x40000000, 0x1000, 0);
    *(s32*)(c + 0x3d8) = 1;
    *(s32*)(c + 0x3dc) = 1;
}
