typedef int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned char u8;
typedef struct { s32 x, y, z; } Vector3;

extern void func_020383fc(void *);
extern void func_ov020_02113148(void *, void *, void *, int, int, int, int);
extern void func_0201267c(int, void *);
extern void *_ZN5Actor13ClosestPlayerEv(void *);
extern void Vec3_Sub(Vector3 *res, const Vector3 *v0, const Vector3 *v1);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
extern s32 Vec3_HorzLen(const Vector3 *v0);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *);
extern void func_ov020_021131f8(void *);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void *);
extern void *_ZNK12WithMeshClsn13GetWallResultEv(void *);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *, Vector3 *);
extern int _ZN5Actor14GetSubtractionEss(void *, s16, s16);
extern void AddVec3(void *, void *, void *);
extern void *_ZN5Actor10FindWithIDEj(u32);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *, const Vector3 *, u32, int, u32, u32, u32);

extern s16 SINE_TABLE[];

typedef struct {
    u8 _pad[0x9e];
    u16 counter;
    u16 timer;
} State300;

#define A92 (*(u16 *)((char *)c + 0x92))
#define A94 (*(u16 *)((char *)c + 0x94))
#define ST  ((State300 *)((char *)c + 0x300))

void func_ov020_02112b00(char *c)
{
    func_020383fc(c + 0x1bc);
    (*(u16 *)(((int)c + 0x39e) & 0xFFFFFFFFFFFFFFFF))++;

    if (ST->counter <= 0x46) {
        if (ST->counter < 0x32)
            *(s32 *)(c + 0xa8) = 0x6000;
        else
            *(s32 *)(c + 0xa8) = 0;
        func_ov020_02113148(c, c + 0x8c, c + 0x398, -0xfa0, 0xc8, 0x14, 2);
        func_ov020_02113148(c, c + 0x90, c + 0x39c, 0, 0, 0x14, 1);
        if (ST->counter == 0x46) {
            func_0201267c(0x5c, c + 0x74);
        }
    } else {
        if (ST->timer != 0) {
            (*(u16 *)(((int)c + 0x3a0) & 0xFFFFFFFFFFFFFFFF))--;
            if (ST->timer == 0) {
                void *player;
                func_0201267c(0x5d, c + 0x74);
                player = _ZN5Actor13ClosestPlayerEv(c);
                if (player == 0)
                    return;
                {
                    Vector3 *pp = (Vector3 *)(((int)player + 0x5c) & 0xFFFFFFFFFFFFFFFF);
                    Vector3 playerPos;
                    Vector3 diff;
                    s32 r;
                    playerPos.x = pp->x;
                    playerPos.y = pp->y;
                    playerPos.z = pp->z;
                    Vec3_Sub(&diff, &playerPos, (Vector3 *)(c + 0x5c));
                    *(s16 *)(c + 0x94) = _ZN4cstd5atan2E5Fix12IiES1_(diff.x, diff.z);
                    *(s16 *)(c + 0x92) = _ZN4cstd5atan2E5Fix12IiES1_(diff.y, Vec3_HorzLen(&diff)) * -1;
                    r = (s32)(((long long)SINE_TABLE[(A92 >> 4) * 2 + 1] * 0x32000 + 0x800) >> 12);
                    *(s32 *)(c + 0xa4) = (s32)(((long long)r * SINE_TABLE[(A94 >> 4) * 2] + 0x800) >> 12);
                    *(s32 *)(c + 0xa8) = (s32)(((long long)SINE_TABLE[(A92 >> 4) * 2] * -0x32000 + 0x800) >> 12);
                    *(s32 *)(c + 0xac) = (s32)(((long long)r * SINE_TABLE[(A94 >> 4) * 2 + 1] + 0x800) >> 12);
                }
            } else {
                if (ST->timer > 0x14) {
                    *(s16 *)(((int)c + 0x8e) & 0xFFFFFFFFFFFFFFFF) += 0x2710;
                }
            }
        } else {
            if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x1bc) != 0) {
                func_ov020_021131f8(c);
            } else if (_ZNK12WithMeshClsn8IsOnWallEv(c + 0x1bc) != 0) {
                void *wr = _ZNK12WithMeshClsn13GetWallResultEv(c + 0x1bc);
                Vector3 normal;
                _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char *)wr + 4, &normal);
                if (_ZN5Actor14GetSubtractionEss(c, *(s16 *)(c + 0x94),
                        _ZN4cstd5atan2E5Fix12IiES1_(normal.x, normal.z)) > 0x4000) {
                    func_ov020_021131f8(c);
                }
            }
        }
    }

    AddVec3(c + 0x5c, c + 0xa4, c + 0x5c);

    {
        u32 id = *(u32 *)(c + 0x1a0);
        void *a;
        int b;
        if (id == 0)
            return;
        a = _ZN5Actor10FindWithIDEj(id);
        if (a == 0)
            return;
        b = (int)(*(u16 *)((char *)a + 0xc) == 0xbf);
        if (b == 0)
            return;
        b = *(u8 *)((char *)a + 0x6fb);
        if (b != 0)
            return;
        {
            Vector3 pos;
            pos.x = *(s32 *)(c + 0x5c);
            pos.y = *(s32 *)(c + 0x60);
            pos.z = *(s32 *)(c + 0x64);
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &pos, 2, 0xc000, 1, 0, 1);
        }
    }
}
