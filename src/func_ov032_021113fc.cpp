//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

struct Vec3 { s32 x, y, z; };

extern "C" {
    void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void* c, void* v);
    void* _ZN5Actor10FindWithIDEj(u32 id);
    void _ZN5Enemy22SpawnMegaCharParticlesER5ActorPc(void* self, void* a, void* p);
    void _ZN5Actor8PoofDustEv(void* self);
    void _ZN6Player16IncMegaKillCountEv(void* p);
    void func_02012694(int a, void* p);
    void _ZN5Actor24KillAndTrackInDeathTableEv(void* self);
    int func_ov002_020c5cd8(void* a, void* self);
    void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, void* v, u32 a, int f, u32 c, u32 d, u32 e);
}

extern Vec3 data_ov032_021137cc;
extern Vec3 data_ov032_021137d8;
extern int data_ov032_02113abc[];
extern int data_ov032_02113a7c[];

extern "C" void func_ov032_021113fc(void* self)
{
    u8* c = (u8*)self;

    Vec3 v1;
    v1.x = data_ov032_021137cc.x;
    v1.y = data_ov032_021137cc.y;
    v1.z = data_ov032_021137cc.z;
    _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3((void*)(c+0x110), &v1);

    u32 id1 = *(u32*)(c+0x134);
    if (id1 != 0) {
        u8* f = (u8*)_ZN5Actor10FindWithIDEj(id1);
        int isbf = (int)(*(u16*)(f+0xc) == 0xbf);
        if (isbf) {
            if (*(u8*)(f+0x6fb) != 0) return;
            if (*(s32*)(c+0x130) & 0x10) {
                _ZN5Enemy22SpawnMegaCharParticlesER5ActorPc(self, f, 0);
                _ZN5Actor8PoofDustEv(self);
                _ZN6Player16IncMegaKillCountEv(f);
                func_02012694(0x1e, (void*)(c+0x74));
                _ZN5Actor24KillAndTrackInDeathTableEv(self);
                return;
            }
            if (*(u8*)(c+0x428) != 0) {
                if (func_ov002_020c5cd8(f, self) == 1) {
                    func_02012694(0xf6, (void*)(c+0x74));
                    return;
                }
            }
        }
    }

    Vec3 v2;
    v2.x = data_ov032_021137d8.x;
    v2.y = data_ov032_021137d8.y;
    v2.z = data_ov032_021137d8.z;
    _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3((void*)(c+0x150), &v2);

    u32 id2 = *(u32*)(c+0x174);
    if (id2 == 0) return;
    u8* f2 = (u8*)_ZN5Actor10FindWithIDEj(id2);
    int isbf2 = (int)(*(u16*)(f2+0xc) == 0xbf);
    if (isbf2 == 0) return;

    if (*(s32*)(c+0x170) & 0x10) {
        _ZN5Enemy22SpawnMegaCharParticlesER5ActorPc(self, f2, 0);
        _ZN5Actor8PoofDustEv(self);
        _ZN6Player16IncMegaKillCountEv(f2);
        _ZN5Actor24KillAndTrackInDeathTableEv(self);
        return;
    }

    int* s3b0 = *(int**)(c+0x3b0);
    if (s3b0 == data_ov032_02113abc) return;
    if (s3b0 == data_ov032_02113a7c) return;

    Vec3 hv;
    hv.x = *(s32*)(c+0x5c);
    hv.y = *(s32*)(c+0x60);
    hv.z = *(s32*)(c+0x64);
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(f2, &hv, 2, 0xc000, 1, 0, 1);
}
