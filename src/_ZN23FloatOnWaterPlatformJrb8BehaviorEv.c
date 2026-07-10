typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

struct Vector3 { int x, y, z; };

extern short data_02082214[];

extern void *_ZN5Actor15FindWithActorIDEjPS_(unsigned int id, void *p);
extern void _ZN9ActorBase18MarkForDestructionEv(void *self);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *clsn);
extern void func_020383fc(void *p);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
extern void *_ZNK12WithMeshClsn14GetFloorResultEv(void *self);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *self, struct Vector3 *out);
extern int _ZN4cstd4fdivEii(int a, int b);
extern int _ZN5Actor13DistToCPlayerEv(void *self);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int, unsigned int, unsigned int, void *, unsigned int);
extern void func_ov016_021130a4(char *t);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);

int _ZN23FloatOnWaterPlatformJrb8BehaviorEv(char *a)
{
    struct Vector3 normal;

    switch (*(u8 *)(a + 0x4f4)) {
    case 0:
        *(void **)(a + 0x320) = _ZN5Actor15FindWithActorIDEjPS_(0x39, 0);
        if (*(void **)(a + 0x320) == 0) {
            _ZN9ActorBase18MarkForDestructionEv(a);
            break;
        }
        (*(u8 *)(((int)a + 0x4f4) & 0xFFFFFFFFFFFFFFFF))++;
        /* fallthrough */
    case 1:
        _ZN5Actor9UpdatePosEP12CylinderClsn(a, 0);
        func_020383fc(a + 0x324);
        if (_ZNK12WithMeshClsn10IsOnGroundEv(a + 0x324)) {
            (*(u8 *)(((int)a + 0x4f4) & 0xFFFFFFFFFFFFFFFF))++;
            *(int *)(a + 0x4e0) = *(int *)(a + 0x5c);
            *(int *)(a + 0x4e4) = *(int *)(a + 0x60);
            *(int *)(a + 0x4e8) = *(int *)(a + 0x64);
        }
        break;
    case 2: {
        s16 *src = (s16 *)(((int)*(char **)(a + 0x320) + 0x8c) & 0xFFFFFFFFFFFFFFFF);
        int spd;
        *(s16 *)(a + 0x8c) = src[0];
        *(s16 *)(a + 0x8e) = src[1];
        *(s16 *)(a + 0x90) = src[2];
        *(s16 *)(a + 0x94) = *(s16 *)(a + 0x8e);
        *(int *)(a + 0x98) = data_02082214[(*(u16 *)(a + 0x8c) >> 4) * 2] * 0x8c;
        *(int *)(((int)a + 0x4ec) & 0xFFFFFFFFFFFFFFFF) += *(int *)(a + 0x98);
        spd = *(int *)(a + 0x4ec);
        if (spd >= 0x4ff000)
            *(int *)(a + 0x4ec) = 0x4ff000;
        else if (spd < -0x32000)
            *(int *)(a + 0x4ec) = -0x32000;
        *(int *)(a + 0x5c) = *(int *)(a + 0x4e0) + (int)(((long long)*(int *)(a + 0x4ec) * data_02082214[(*(u16 *)(a + 0x8e) >> 4) * 2] + 0x800) >> 12);
        *(int *)(a + 0x64) = *(int *)(a + 0x4e8) + (int)(((long long)*(int *)(a + 0x4ec) * data_02082214[(*(u16 *)(a + 0x8e) >> 4) * 2 + 1] + 0x800) >> 12);
        *(int *)(a + 0x98) = 0;
        _ZN5Actor9UpdatePosEP12CylinderClsn(a, 0);
        func_020383fc(a + 0x324);
        if (_ZNK12WithMeshClsn10IsOnGroundEv(a + 0x324)) {
            void *fr = _ZNK12WithMeshClsn14GetFloorResultEv(a + 0x324);
            _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char *)fr + 4, &normal);
            if (normal.y != 0) {
                *(int *)(a + 0xa8) = -(_ZN4cstd4fdivEii(
                    (int)(((long long)normal.x * *(int *)(a + 0xa4) + 0x800) >> 12)
                  + (int)(((long long)normal.z * *(int *)(a + 0xac) + 0x800) >> 12),
                    normal.y) + 0x8000);
            }
        }
        if (_ZN5Actor13DistToCPlayerEv(a) < 0x7d0000) {
            int vel = *(int *)(a + 0x98);
            if (vel < 0)
                vel = -vel;
            if (vel > 0x3000) {
                *(int *)(a + 0x4f0) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(unsigned int *)(a + 0x4f0), 3, 0x9f, a + 0x74, 0);
            }
        }
        break;
    }
    }

    func_ov016_021130a4(a);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(a, 0, 0)) {
        _ZN8Platform19UpdateClsnPosAndRotEv(a);
    }
    return 1;
}
