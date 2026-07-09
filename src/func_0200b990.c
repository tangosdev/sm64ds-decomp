typedef unsigned char u8;
typedef short s16;

typedef struct V3 { int x, y, z; } V3;
typedef struct RaycastLine { char pad[0x14]; char surf[0x64]; } RaycastLine;

extern s16 Vec3_HorzAngle(const V3 *a, const V3 *b);
extern void _ZN11RaycastLineC1Ev(RaycastLine *rc);
extern void func_0200897c(char *self, RaycastLine *rc);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(RaycastLine *rc, V3 *a, V3 *b, void *actor);
extern int _ZN11RaycastLine10DetectClsnEv(RaycastLine *rc);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *surf, V3 *out);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
extern int AngleDiff(int a, int b);
extern void _ZN11RaycastLineD1Ev(RaycastLine *rc);
extern void _Z14ApproachLinearRsss(s16 *v, s16 step, s16 rate);
extern signed char data_0209f2f8;

void func_0200b990(char *self, char *arg1, int arg2)
{
    V3 v;
    s16 angle;
    u8 b0, b1;
    int step;
    int rate;

    {
        int sz = (int)(*(s16 *)(arg1 + 6)) << 12;
        int sy = (int)(*(s16 *)(arg1 + 4)) << 12;
        int sx = (int)(*(s16 *)(arg1 + 2)) << 12;
        v.x = sx;
        v.y = sy;
        v.z = sz;
    }
    angle = Vec3_HorzAngle(&v, (V3 *)(self + 0x80));

    rate = 8;
    b0 = *(u8 *)(arg1 + 0);
    if (b0 == 0) {
        b1 = *(u8 *)(arg1 + 1);
        if (b1 == 0xff) {
            signed char zone = data_0209f2f8;
            if (zone != 0x16 && zone != 0xa && zone != 0x10) {
                RaycastLine rc;
                V3 lineB;
                _ZN11RaycastLineC1Ev(&rc);
                func_0200897c(self, &rc);
                lineB.y = *(int *)(self + 0x84);
                lineB.x = v.x;
                lineB.z = v.z;
                _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(&rc, (V3 *)(self + 0x80), &lineB, 0);
                if (_ZN11RaycastLine10DetectClsnEv(&rc) != 0) {
                    V3 normal;
                    s16 a2;
                    _ZNK11SurfaceInfo12CopyNormalToER7Vector3(rc.surf, &normal);
                    a2 = _ZN4cstd5atan2E5Fix12IiES1_(normal.x, normal.z);
                    if (AngleDiff(a2, angle) < 0x1800) angle = a2;
                }
                _ZN11RaycastLineD1Ev(&rc);
            }
            step = 0x2000;
        } else {
            step = 0;
            rate = (data_0209f2f8 == 0x2d) ? 0x12 : 0xf;
        }
    } else {
        step = 0x6800;
    }

    {
        char *ptr = *(char **)(self + 0x110);
        s16 target = *(s16 *)(ptr + 0x8e);
        s16 diff = (s16)(angle - target);
        if (diff < 0) {
            angle -= step;
        } else {
            angle += step;
        }
    }

    {
        int fixmul = (int)(((long long)arg2 * rate + 0x800) >> 12);
        _Z14ApproachLinearRsss((s16 *)(self + 0x17c), angle, (s16)fixmul);
    }
}
