//cpp
extern "C" {
struct Vector3 { int x, y, z; };
extern short Vec3_HorzAngle(const void *a, const void *b);
extern int Vec3_HorzDist(const void *a, const void *b);
extern int Vec3_Dist(const void *a, const void *b);
extern unsigned _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned a, unsigned b, unsigned c, const void *v, unsigned e);
extern void *_ZN5Actor13ClosestPlayerEv(void *self);
extern void _Z14ApproachLinearRsss(short *v, short target, short step);
extern int func_ov002_020de328(void *p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *clsn);
extern void func_020383fc(void *p);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void *p);
extern void *_ZNK12WithMeshClsn13GetWallResultEv(void *p);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *s, int *out);
extern short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern void func_ov096_02136e54(void *self, int n);
extern unsigned _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned a, unsigned b, int x, int y, int z, const void *v, void *cb);
void func_ov096_02137088(char *c);
}
#define M(p) ((long long)(int)(p) & 0xFFFFFFFFFFFFFFFFLL)

void func_ov096_02137088(char *c)
{
    struct Vector3 pos;
    char *player;
    void *p33c;
    short ang;
    unsigned short *p354 = (unsigned short *)(int)M(c + 0x354);
    *p354 = (unsigned short)(*p354 + 1);

    ang = Vec3_HorzAngle(c + 0x5c, c + 0x340);
    *(short *)(c + 0x356) = ang;

    *(unsigned *)(c + 0x36c) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
        *(unsigned *)(c + 0x36c), 3, 0x85, c + 0x74, 0);

    *(int *)(c + 0x98) = 0x14000;

    player = (char *)_ZN5Actor13ClosestPlayerEv(c);
    if (player == 0)
        goto null_player;

    {
        int *pp = (int *)(int)M(player + 0x5c);
        pos.x = pp[0];
        pos.y = pp[1];
        pos.z = pp[2];
    }

    if (Vec3_HorzDist(c + 0x340, &pos) < *(int *)(c + 0x34c)
        && *(unsigned char *)(c + 0x360) == 0
        && *(unsigned short *)(c + 0x354) < 0x384) {
        ang = Vec3_HorzAngle(c + 0x5c, &pos);
        *(short *)(c + 0x358) = ang;
        _Z14ApproachLinearRsss((short *)(c + 0x94), *(short *)(c + 0x358), 0x200);
        p33c = *(void **)(c + 0x33c);
        if (p33c != 0) {
            if (func_ov002_020de328(p33c) != 0) {
                unsigned char *pb = (unsigned char *)(int)M(c + 0x360);
                *pb = (unsigned char)(*pb + 1);
            }
        }
    } else {
        _Z14ApproachLinearRsss((short *)(c + 0x94), *(short *)(c + 0x356), 0x200);
        if (Vec3_HorzDist(c + 0x340, c + 0x5c) < 0xc8000)
            *(int *)(c + 0x35c) = 2;
    }
    goto cont;

null_player:
    *(int *)(c + 0x35c) = 2;
    return;

cont:
    if (Vec3_Dist(c + 0x5c, &pos) > 0xbb8000
        || *(unsigned short *)(c + 0x354) >= 0x384) {
        *(int *)(c + 0x35c) = 2;
    }

    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
    func_020383fc(c + 0x108);
    if (_ZNK12WithMeshClsn8IsOnWallEv(c + 0x108) != 0) {
        int n[3];
        void *wr = _ZNK12WithMeshClsn13GetWallResultEv(c + 0x108);
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char *)wr + 4, n);
        *(short *)(c + 0x94) = _ZN4cstd5atan2E5Fix12IiES1_(n[0], n[2]);
    }

    func_ov096_02136e54(c, 0x1000);

    {
        int z = *(int *)(c + 0x64);
        *(unsigned *)(c + 0x364) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(unsigned *)(c + 0x364), 0x11f,
            *(int *)(c + 0x5c), *(int *)(c + 0x60), z, 0, 0);
        z = *(int *)(c + 0x64);
        *(unsigned *)(c + 0x368) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(unsigned *)(c + 0x368), 0x120,
            *(int *)(c + 0x5c), *(int *)(c + 0x60), z, 0, 0);
    }
}
