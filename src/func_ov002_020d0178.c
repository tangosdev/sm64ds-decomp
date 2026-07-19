typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

typedef struct Vec3 { int x, y, z; } Vec3;

extern void _ZN13RaycastGroundC1Ev(void* self);
extern void _ZN13RaycastGroundD1Ev(void* self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void* self, Vec3* pos, void* actor);
extern int _ZN13RaycastGround10DetectClsnEv(void* self);
extern void _ZN11RaycastLineC1Ev(void* self);
extern void _ZN11RaycastLineD1Ev(void* self);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void* self, Vec3* a, Vec3* b, void* actor);
extern int _ZN11RaycastLine10DetectClsnEv(void* self);
extern void _ZN11RaycastLine10GetClsnPosEv(Vec3* out, void* self);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* self, Vec3* out);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int x, int z);
extern int AngleDiff(int a, int b);
extern s16 data_02082214[];

int func_ov002_020d0178(char* c, int arg1, int flag)
{
    Vec3 normal;
    Vec3 pos1;
    Vec3 lineStart;
    Vec3 clsnPos;
    Vec3 line2;
    char rg[0x50];
    char rl[0x78];
    int ang;
    int gy;
    int res;

    _ZN13RaycastGroundC1Ev(rg);
    if (flag != 0)
        ang = (s16)(*(s16*)(c + 0x8e) + 0x8000);
    else
        ang = *(s16*)(c + 0x8e);
    {
        u8 b7 = *(u8*)(c + 0x703);
        int d = 0x40;
        int i;
        int s, csn;
        int xoff, zoff;
        int h;
        if (b7 != 0)
            d = 0xd0;
        i = (((u16)ang) >> 4) * 2;
        d <<= 12;
        s = data_02082214[i];
        csn = data_02082214[i + 1];
        xoff = (int)(((long long)d * s + 0x800) >> 12);
        zoff = (int)(((long long)d * csn + 0x800) >> 12);
        h = 0xc8;
        if (b7 != 0)
            h = 0x1f4;
        {
            int tz = *(int*)(c + 0x64) + zoff;
            int ty = *(int*)(c + 0x60) + (h << 12);
            int tx = *(int*)(c + 0x5c) + xoff;
            pos1.x = tx;
            pos1.y = ty;
            pos1.z = tz;
        }
    }
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(rg, &pos1, c);
    if (!_ZN13RaycastGround10DetectClsnEv(rg)) {
        _ZN13RaycastGroundD1Ev(rg);
        return 0;
    }
    gy = *(int*)(rg + 0x44);
    if (flag != 0) {
        if (gy > *(int*)(c + 0x60) + 0x64000 || gy < *(int*)(c + 0x60)) {
            _ZN13RaycastGroundD1Ev(rg);
            return 0;
        }
    } else {
        int lo = 0x64;
        if (*(u8*)(c + 0x703) != 0)
            lo = 0x190;
        if (gy > *(int*)(c + 0x60) + ((lo + 0x64) << 12) || gy < *(int*)(c + 0x60) + (lo << 12)) {
            _ZN13RaycastGroundD1Ev(rg);
            return 0;
        }
    }
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3(rg + 0x14, &normal);
    if (normal.y < 0xf04) {
        _ZN13RaycastGroundD1Ev(rg);
        return 0;
    }
    _ZN11RaycastLineC1Ev(rl);
    pos1.y = gy;
    {
        int tz = *(int*)(c + 0x64);
        int tx = *(int*)(c + 0x5c);
        lineStart.x = tx;
        lineStart.y = arg1;
        lineStart.z = tz;
    }
    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(rl, &lineStart, &pos1, c);
    if (!_ZN11RaycastLine10DetectClsnEv(rl)) {
        _ZN11RaycastLineD1Ev(rl);
        _ZN13RaycastGroundD1Ev(rg);
        return 0;
    }
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3(rl + 0x14, &normal);
    if (normal.y >= 0x2c7) {
        _ZN11RaycastLineD1Ev(rl);
        _ZN13RaycastGroundD1Ev(rg);
        return 0;
    }
    res = _ZN4cstd5atan2E5Fix12IiES1_(normal.x, normal.z);
    {
        int diff = AngleDiff(res, *(s16*)(c + 0x8e));
        if (flag != 0) {
            if (diff < 0)
                diff = -diff;
            if (diff > 0x4000) {
                _ZN11RaycastLineD1Ev(rl);
                _ZN13RaycastGroundD1Ev(rg);
                return 0;
            }
        } else {
            if (diff < 0)
                diff = -diff;
            if (diff < 0x4000) {
                _ZN11RaycastLineD1Ev(rl);
                _ZN13RaycastGroundD1Ev(rg);
                return 0;
            }
        }
    }
    _ZN11RaycastLine10GetClsnPosEv(&clsnPos, rl);
    lineStart.y += 0x64000;
    pos1.y += 0x64000;
    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(rl, &lineStart, &pos1, c);
    if (_ZN11RaycastLine10DetectClsnEv(rl)) {
        _ZN11RaycastLineD1Ev(rl);
        _ZN13RaycastGroundD1Ev(rg);
        return 0;
    }
    lineStart.y = *(int*)(c + 0x60);
    line2.x = *(int*)(c + 0x5c);
    line2.y = *(int*)(c + 0x60);
    line2.z = *(int*)(c + 0x64);
    line2.y = gy + 0xa000;
    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(rl, &lineStart, &line2, c);
    if (_ZN11RaycastLine10DetectClsnEv(rl)) {
        _ZN11RaycastLineD1Ev(rl);
        _ZN13RaycastGroundD1Ev(rg);
        return 0;
    }
    *(int*)(c + 0x5c) = clsnPos.x - normal.x;
    *(int*)(c + 0x64) = clsnPos.z - normal.z;
    *(int*)(c + 0x60) = gy;
    *(s16*)(c + 0x8e) = (u16)(res + 0x8000);
    _ZN11RaycastLineD1Ev(rl);
    _ZN13RaycastGroundD1Ev(rg);
    return 1;
}
