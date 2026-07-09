//cpp
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

extern "C" {

struct V16 { u16 x, y, z; };

extern int _ZNK12WithMeshClsn8IsOnWallEv(void* p);
extern void* _ZNK12WithMeshClsn13GetWallResultEv(void* p);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* si, void* out);
extern s16 _ZN5Actor12ReflectAngleE5Fix12IiES1_s(void* self, int a, int b, short c);
extern void* _ZN5Actor10FindWithIDEj(unsigned id);
extern int _ZN6Player15IsCollectingCapEv(void* p);
extern void _ZN6Player15InitVanishLuigiEv(void* p);
extern void _ZN9ActorBase18MarkForDestructionEv(void* self);
extern void _ZN6Player14InitMetalWarioEv(void* p);
extern int _ZN8SaveData16HasPlayerLostCapEv(void);
extern void func_ov002_020b7f2c(void* c, void* pmf);
extern int _ZN6Player17SetNoControlStateEhih(void* p, u8 a, int b, u8 c);
extern void _ZN6Player18SetNewHatCharacterEjjb(void* p, unsigned a, unsigned b, bool c);

extern int data_ov002_0210df54;

void func_ov002_020b6fcc(char* self)
{
    struct V16 v;
    int normal[3];
    int state;

    if (_ZNK12WithMeshClsn8IsOnWallEv(self + 0x144) != 0) {
        void* wr = _ZNK12WithMeshClsn13GetWallResultEv(self + 0x144);
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)wr + 4, &normal[0]);
        *(s16*)(self + 0x94) = _ZN5Actor12ReflectAngleE5Fix12IiES1_s(
            self, normal[0], normal[2], *(s16*)(self + 0x94));
    }

    if (*(int*)(self + 0x134) == 0) return;

    *(void**)(self + 0x3c0) = _ZN5Actor10FindWithIDEj(*(int*)(self + 0x134));
    if (*(void**)(self + 0x3c0) == 0) return;

    {
        int t = (*(u16*)(*(char**)(self + 0x3c0) + 0xc) == 0xbf);
        if (t == false) return;
    }

    if ((*(int*)(self + 0x130) & 0x8000) != 0) return;
    if (_ZN6Player15IsCollectingCapEv(*(void**)(self + 0x3c0)) != 0) return;

    if (*(int*)(*(char**)(self + 0x3c0) + 8) == 3) {
        if (*(int*)(*(char**)(self + 0x3c0) + 0x360) != 0) return;
    }

    state = *(int*)(self + 0x3f0);
    if ((unsigned)(state - 6) <= 1) {
        _ZN6Player15InitVanishLuigiEv(*(void**)(self + 0x3c0));
        _ZN9ActorBase18MarkForDestructionEv(self);
        return;
    }
    if ((unsigned)(state - 8) <= 1) {
        _ZN6Player14InitMetalWarioEv(*(void**)(self + 0x3c0));
        _ZN9ActorBase18MarkForDestructionEv(self);
        return;
    }

    if (*(int*)(self + 0xc8) != 0) {
        if (state != 0) return;
    }

    if (_ZN8SaveData16HasPlayerLostCapEv() != 0) {
        func_ov002_020b7f2c(self, &data_ov002_0210df54);
        *(u8*)(self + 0x401) = 1;
        return;
    }

    if (_ZN6Player17SetNoControlStateEhih(*(void**)(self + 0x3c0), 8, -1, 0) == 0) return;

    _ZN6Player18SetNewHatCharacterEjjb(
        *(void**)(self + 0x3c0), *(int*)(self + 0x3f4) & 0xff, 0, 0);

    {
        char* found = *(char**)(self + 0x3c0);
        int a = *(u16*)(found + 0x8c);
        int b = *(u16*)(found + 0x8e);
        a = b ? a : a;
        v.x = a;
        v.y = b;
        v.z = *(u16*)(found + 0x90);
        *(s16*)(self + 0x92) = *(s16*)&v.x;
        *(s16*)(self + 0x94) = *(s16*)&v.y;
        *(s16*)(self + 0x96) = *(s16*)&v.z;
        *(s16*)(self + 0x8c) = *(s16*)(self + 0x92);
        *(s16*)(self + 0x8e) = *(s16*)(self + 0x94);
        *(s16*)(self + 0x90) = *(s16*)(self + 0x96);
    }

    func_ov002_020b7f2c(self, &data_ov002_0210df54);
}

}
