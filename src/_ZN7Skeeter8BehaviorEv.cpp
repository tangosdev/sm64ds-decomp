//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

struct Vector3 { s32 x, y, z; };

class Actor {};
typedef void (Actor::*ActorFn)();
struct PmfNode { char pad[8]; ActorFn fn; };

extern "C" {
int  _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(void* self, void* wm);
void _ZN12CylinderClsn5ClearEv(void* self);
void _ZN12CylinderClsn6UpdateEv(void* self);
void func_ov090_02131e50(void* c);
int  _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(void* self, void* wm, void* anim, u32 n);
void func_02035684(int* p, int v);
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void* self, void* wm, u32 n);
int  _ZN5Enemy11UpdateDeathER12WithMeshClsn(void* self, void* wm);
void func_ov090_02131378(void* c);
void _ZN5Actor8PoofDustEv(void* self);
void _Z14ApproachLinearRsss(s16* p, s16 a, s16 b);
int  AngleDiff(int a, int b);
void _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(void* self, const void* v, u32 n, s32 fix, s16 s);
void _ZN5Actor24KillAndTrackInDeathTableEv(void* self);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* cyl);
int  func_02035638(u8* p);
void* _ZN5Actor13ClosestPlayerEv(void* self);
void func_020aea30(void* self, void* p, int n);
u16  DecIfAbove0_Short(u16* p);
void _ZN9Animation7AdvanceEv(void* self);
void func_ov090_021310b4(void* c);
extern signed char data_0209f2f8;
}

extern "C" int _ZN7Skeeter8BehaviorEv(void* self)
{
    char* c = (char*)self;

    if (_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(c, c + 0x150)) {
        _ZN12CylinderClsn5ClearEv(c + 0x110);
        if (*(u8*)(c + 0x107) != 0 && *(u16*)(c + 0x104) == 0)
            _ZN12CylinderClsn6UpdateEv(c + 0x110);
        func_ov090_02131e50(c);
        return 1;
    }

    if (_ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(c, c + 0x150, c + 0x30c, 3))
        return 1;

    if (*(s32*)(c + 0x10c) != 0) {
        func_02035684((int*)(c + 0x150), 0xd2000);
        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x150, 0);
        if (_ZN5Enemy11UpdateDeathER12WithMeshClsn(c, c + 0x150))
            return 1;
        func_ov090_02131378(c);
        func_ov090_02131e50(c);
        if (*(s32*)(c + 0x10c) == 0)
            _ZN5Actor8PoofDustEv(c);
        if (*(u8*)(c + 0x3a1) == 3) {
            _Z14ApproachLinearRsss((s16*)(c + 0x8c), -32767, 0x500);
            if (AngleDiff(*(s16*)(c + 0x8c), -32767) < 0x1000) {
                s16* p8e = (s16*)(((int)c + 0x8e) & 0xFFFFFFFFFFFFFFFF);
                *p8e += 0x1000;
            }
        }
        if (*(s32*)(c + 0x10c) != 1 && *(s32*)(c + 0x60) <= *(s32*)(c + 0x3ac)) {
            Vector3 v;
            v.x = *(s32*)(c + 0x5c);
            v.y = *(s32*)(c + 0x60);
            v.z = *(s32*)(c + 0x64);
            _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(c, &v, *(u8*)(c + 0x10a) + 1, 0xa000, 0);
            _ZN5Actor8PoofDustEv(c);
            _ZN5Actor24KillAndTrackInDeathTableEv(c);
        }
        return 1;
    }

    {
    int flag = (*(s32*)(c + 0xb0) & 8) != 0;
    if (flag) {
        *(s32*)(c + 0x98) = 0;
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x110);
        func_ov090_02131378(c);
        if (data_0209f2f8 == 0x15 && *(signed char*)(c + 0xcc) == 1) {
            _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x150, 2);
            if (func_02035638((u8*)(c + 0x150))) {
                *(s32*)(c + 0x9c) = 0;
                *(s32*)(c + 0xa4) = 0;
                *(s32*)(c + 0xa8) = 0;
                *(s32*)(c + 0xac) = 0;
                *(s32*)(c + 0x10c) = 1;
                func_020aea30(c, _ZN5Actor13ClosestPlayerEv(c), 0);
                return 1;
            }
        }
        return 1;
    }
    }

    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x110);
    func_ov090_02131378(c);
    DecIfAbove0_Short((u16*)(c + 0x100));
    DecIfAbove0_Short((u16*)(c + 0x394));
    DecIfAbove0_Short((u16*)(c + 0x396));
    DecIfAbove0_Short((u16*)(c + 0x398));
    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x150, 2);
    if (*(s32*)(c + 0x60) <= *(s32*)(c + 0x3ac))
        *(s32*)(c + 0x60) = *(s32*)(c + 0x3ac);
    if (data_0209f2f8 == 0x15 && *(signed char*)(c + 0xcc) == 1 && func_02035638((u8*)(c + 0x150))) {
        *(s32*)(c + 0x9c) = 0;
        *(s32*)(c + 0xa4) = 0;
        *(s32*)(c + 0xa8) = 0;
        *(s32*)(c + 0xac) = 0;
        *(s32*)(c + 0x10c) = 1;
        func_020aea30(c, _ZN5Actor13ClosestPlayerEv(c), 0);
        return 1;
    }

    {
        PmfNode* n = *(PmfNode**)(c + 0x370);
        if (n->fn)
            (((Actor*)c)->*(n->fn))();
    }
    *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
    *(s32*)(c + 0x368) = *(s32*)(c + 0x3a4);
    _ZN9Animation7AdvanceEv(c + 0x35c);
    func_ov090_02131e50(c);
    func_ov090_021310b4(c);
    _ZN12CylinderClsn5ClearEv(c + 0x110);
    {
        void* p = _ZN5Actor13ClosestPlayerEv(c);
        if (p != 0 && *(u8*)((char*)p + 0x6fb) == 0)
            _ZN12CylinderClsn6UpdateEv(c + 0x110);
    }
    return 1;
}
