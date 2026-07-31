//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

struct Vec3 {
    int x, y, z;
    Vec3() {}
    ~Vec3() {}
};

extern "C" {

extern void WithMeshClsn_UpdateContinuous_Veneer(void* p);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void* p);
extern void* _ZNK12WithMeshClsn14GetFloorResultEv(void* p);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* info, Vec3* out);
extern int _ZNK12WithMeshClsn13JustHitGroundEv(void* p);
extern void* _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int a, int b, int c);
extern void func_0201267c(int a, void* p);
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int a, unsigned int b, int c, int d, int e, void* v, void* cb);
extern void* _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int c, void* v, unsigned int d);
extern int Vec3_HorzLen(void* v);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void* p);
extern void* _ZNK12WithMeshClsn13GetWallResultEv(void* p);
extern int _ZN5Actor14GetSubtractionEss(void* a, s16 x, int y);
extern s16 _ZN5Actor12ReflectAngleE5Fix12IiES1_s(void* a, int x, int y, s16 ang);
extern void _ZN5Actor28UpdatePosWithHorzSpeedAndAngEv(void* a);

extern s16 data_02082214[];

}

extern "C" void func_ov021_02112544(char* self)
{
    Vec3 normal;
    Vec3 pos;
    Vec3 walln;

    WithMeshClsn_UpdateContinuous_Veneer(self + 0x1f8);
    if (_ZNK12WithMeshClsn10IsOnGroundEv(self + 0x1f8) != 0) {
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)_ZNK12WithMeshClsn14GetFloorResultEv(self + 0x1f8) + 4, &normal);

        pos = *(Vec3*)(self + 0x5c);
        pos.x -= normal.x * 0x12c;
        pos.y -= normal.y * 0x12c;
        pos.z -= normal.z * 0x12c;

        if (_ZNK12WithMeshClsn13JustHitGroundEv(self + 0x1f8) != 0) {
            if (*(int*)(self + 0xa8) < (*(int*)(self + 0x9c) << 1) && normal.y >= 0xc00) {
                *(int*)(self + 0xa8) = -*(int*)(self + 0xa8) * 6 / 10;
            }
            pos.y += 0x3c000;
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x68, pos.x, pos.y, pos.z);
            func_0201267c(0x48, self + 0x74);
        } else {
            *(int*)(self + 0x3c4) = (int)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                *(int*)(self + 0x3c4), 0x69, pos.x, pos.y, pos.z, 0, 0);
            *(int*)(self + 0x3b8) = (int)_ZN5Sound8PlayLongEjjjRK7Vector3j(
                *(int*)(self + 0x3b8), 3, 0x8a, self + 0x74, 0);
        }
        {
            int *pa4 = (int *)(int)(((long long)(int)(self + 0xa4)) & 0xFFFFFFFFFFFFFFFFLL);
            int *pac = (int *)(int)(((long long)(int)(self + 0xac)) & 0xFFFFFFFFFFFFFFFFLL);
            *pa4 = *pa4 + normal.x * 5;
            *pac = *pac + normal.z * 5;
        }
        *(int*)(self + 0x98) = Vec3_HorzLen(self + 0xa4);
        *(s16*)(self + 0x94) = (s16)_ZN4cstd5atan2E5Fix12IiES1_(*(int*)(self + 0xa4), *(int*)(self + 0xac));
        if (*(int*)(self + 0x98) >= 0x22000) {
            *(int*)(self + 0x98) = 0x22000;
            *(int*)(self + 0xa4) = data_02082214[(*(u16*)(self + 0x94) >> 4) << 1] * 0x22;
            *(int*)(self + 0xac) = data_02082214[((*(u16*)(self + 0x94) >> 4) << 1) + 1] * 0x22;
        }
    }

    if (_ZNK12WithMeshClsn8IsOnWallEv(self + 0x1f8) == 0)
        return;

    _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)_ZNK12WithMeshClsn13GetWallResultEv(self + 0x1f8) + 4, &walln);
    {
        int wa = _ZN4cstd5atan2E5Fix12IiES1_(walln.x, walln.z);
        if (_ZN5Actor14GetSubtractionEss(self, *(s16*)(self + 0x94), wa) <= 0x1000)
            return;
        *(s16*)(self + 0x94) = _ZN5Actor12ReflectAngleE5Fix12IiES1_s(self, walln.x, walln.z, *(s16*)(self + 0x94));
        _ZN5Actor28UpdatePosWithHorzSpeedAndAngEv(self);
    }
}
