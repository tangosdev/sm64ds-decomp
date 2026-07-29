//cpp
typedef int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef struct { s32 x, y, z; } Vector3;
struct PVec { s32 x, y, z; ~PVec() {} };
typedef s32 Fix12;

extern "C" {
extern char *_ZN5Actor10FindWithIDEj(u32 id);
extern int func_ov002_020d85fc(char *self, char *arg, int flags);
extern int func_ov002_020d8360(void *self, void *other, u32 flags);
extern int _ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(char *self, void *clsn, char *player);
extern int _ZN6Player7IsInAirEv(char *p);
extern int func_ov002_020d9298(char *c);
extern void func_ov002_020db8bc(u8 *p, u8 val);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, Fix12 x, Fix12 y, Fix12 z);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, const Vector3 *pos);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, const Vector3 *pos);
}

extern "C" int func_ov002_020d869c(char *c)
{
    char *r6;
    u32 flags;
    u32 f2;
    int t;
    PVec p;
    s32 x;
    s32 y;
    s32 z;

    if (*(s32 *)(c + 0xd0) != 0 || *(u8 *)(c + 0x713) == 0)
        return 0;
    if (*(u32 *)(c + 0x2f8) == 0)
        return 0;
    r6 = _ZN5Actor10FindWithIDEj(*(u32 *)(c + 0x2f8));
    if (r6 == 0 || r6 == c)
        return 0;
    t = (int)(*(u16 *)(r6 + 0xc) == 0xbf);
    if (t == 0)
        return 0;

    flags = *(u32 *)(c + 0x2f4) & 0x27fe0;
    if (flags != 0) {
        if (func_ov002_020d85fc(c, r6, flags) == 1)
            return 0;
        f2 = flags & ~0x1000u;
        if (f2 != 0)
            func_ov002_020d8360(c, r6, f2);
    } else {
        if (*(u8 *)(c + 0x709) != 0)
            return 0;
        if (_ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(c, c + 0x2d4, r6) != 0) {
            if (_ZN6Player7IsInAirEv(r6) == 0)
                return 0;
            if (func_ov002_020d9298(r6) != 0) {
                func_ov002_020db8bc((u8 *)c, 0);
                x = *(s32 *)(c + 0x5c);
                y = *(s32 *)(c + 0x60) + 0x82000;
                z = *(s32 *)(c + 0x64);
                p.x = x;
                p.y = y;
                p.z = z;
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb2, p.x, p.y, p.z);
                _ZN5Sound9PlayBank0EjRK7Vector3(0x12, (const Vector3 *)(c + 0x74));
                _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8 *)(c + 0x6d9), 0x19, (const Vector3 *)(c + 0x74));
            }
        }
    }
    return 1;
}
