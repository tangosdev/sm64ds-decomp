//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef long long s64;
typedef int Fix12;
struct Vector3 { s32 x, y, z; };

extern "C" {
    void* _ZN5Actor10FindWithIDEj(u32 id);
    void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* self, const Vector3& v, u32 a, Fix12 b, u32 c, u32 d, u32 e);
    void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* cc);
    void func_02038408(void* p);
    void func_020383fc(void* p);
    int _ZNK12WithMeshClsn10IsOnGroundEv(void* self);
    void* _ZNK12WithMeshClsn14GetFloorResultEv(void* self);
    void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* self, Vector3* out);
    int _ZNK12WithMeshClsn13JustHitGroundEv(void* self);
    int _ZN4cstd4fdivEii(int a, int b);
    int func_ov060_02112ba8(char* c);
}

struct TabEnt { int off; int idx; };
extern TabEnt data_ov060_0211aed4[];
extern u8 data_ov060_02119268[];
extern s16 data_02082214[];

extern "C" void func_ov060_021128c0(char* c)
{
    *(u8*)(c + 0x425) = 0;
    u32 id;
    if (*(s32*)(c + 0x40c) != 4 && (id = *(u32*)(c + 0x384)) != 0) {
        void* f = _ZN5Actor10FindWithIDEj(id);
        if (f != 0) {
            int b = (*(u16*)((char*)f + 0xc) == 0xbf);
            if (b) {
                Vector3 v;
                v.x = *(s32*)(c + 0x5c);
                v.y = *(s32*)(c + 0x60);
                v.z = *(s32*)(c + 0x64);
                _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(f, v, 2, 0x8000, 1, 0, 1);
            }
        }
    }

    s32 idx = *(s32*)(c + 0x40c);
    {
        TabEnt* e = &data_ov060_0211aed4[idx];
        int off = e->idx;
        void* base = (void*)(c + (off >> 1));
        void (*fn)(void*);
        if (off & 1)
            fn = (void (*)(void*))*(void**)((char*)(*(void***)base) + e->off);
        else
            fn = (void (*)(void*))e->off;
        fn(base);
    }

    *(u16*)(((int)c + 0x3fc) & 0xFFFFFFFFFFFFFFFFLL) =
        *(u16*)(((int)c + 0x3fc) & 0xFFFFFFFFFFFFFFFFLL) + 1;
    if (*(s32*)(c + 0x40c) != idx) {
        *(u8*)(c + 0x423) = 0;
        *(u16*)((char*)(((int)c + 0x300) & 0xFFFFFFFFFFFFFFFFLL) + 0xfc) = 0;
    }

    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x360);

    if (*(u8*)(c + 0x414) == 1)
        func_02038408(c + 0x14c);
    else
        func_020383fc(c + 0x14c);

    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x14c)) {
        void* fr = _ZNK12WithMeshClsn14GetFloorResultEv(c + 0x14c);
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)fr + 4, (Vector3*)(c + 0x3bc));
        *(s32*)(c + 0x3c8) = *(s32*)(c + 0x5c);
        *(s32*)(c + 0x3cc) = *(s32*)(c + 0x60);
        *(s32*)(c + 0x3d0) = *(s32*)(c + 0x64);
        if (*(u8*)(c + 0x427) != 0 && _ZNK12WithMeshClsn13JustHitGroundEv(c + 0x14c)) {
            *(s32*)(c + 0xa8) = (*(s32*)(c + 0xa8) * -60) / 100;
            if (*(s32*)(c + 0xa8) >= 0x14000)
                *(s32*)(c + 0xa8) = 0x14000;
        } else if (*(s32*)(c + 0x3c0) != 0) {
            *(s32*)(c + 0xa8) = -(_ZN4cstd4fdivEii(
                (int)(((s64)*(s32*)(c + 0x3bc) * *(s32*)(c + 0xa4) + 0x800) >> 12)
              + (int)(((s64)*(s32*)(c + 0x3c4) * *(s32*)(c + 0xac) + 0x800) >> 12),
                *(s32*)(c + 0x3c0)) + 0x8000);
        }
    }

    if (data_ov060_02119268[idx] != 0) {
        if (!_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x14c)) {
            char* p400 = (char*)(((int)c + 0x400) & 0xFFFFFFFFFFFFFFFFLL);
            s32* px = (s32*)(((int)c + 0x5c) & 0xFFFFFFFFFFFFFFFFLL);
            s32* pz = (s32*)(((int)c + 0x64) & 0xFFFFFFFFFFFFFFFFLL);
            s16* tab = data_02082214;
            *(s32*)(c + 0x5c) = *(s32*)(c + 0x3c8);
            *(s32*)(c + 0x60) = *(s32*)(c + 0x3cc);
            *(s32*)(c + 0x64) = *(s32*)(c + 0x3d0);
            {
                s32 a = (*(u16*)(p400 + 8) >> 4);
                *px = *px + ((s32)tab[a * 2] << 3);
            }
            {
                s32 a = (*(u16*)(p400 + 8) >> 4);
                *pz = *pz + ((s32)tab[a * 2 + 1] << 3);
            }
        } else {
            *(s32*)(c + 0x3c8) = *(s32*)(c + 0x5c);
            *(s32*)(c + 0x3cc) = *(s32*)(c + 0x60);
            *(s32*)(c + 0x3d0) = *(s32*)(c + 0x64);
        }
    }

    if (func_ov060_02112ba8(c) == 0)
        return;
    *(s32*)(c + 0x40c) = 2;
    *(u8*)(c + 0x423) = 0;
    *(u16*)((char*)(((int)c + 0x300) & 0xFFFFFFFFFFFFFFFFLL) + 0xfc) = 0;
}
