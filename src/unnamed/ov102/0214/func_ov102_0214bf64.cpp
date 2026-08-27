//cpp
typedef int Fix12;
typedef short s16;
struct Vector3 { Fix12 x, y, z; };
extern "C" unsigned short DecIfAbove0_Short(unsigned short* p);
extern "C" void _Z14ApproachLinearRiii(int& v, int target, int step);
extern "C" void _Z14ApproachLinearRsss(short& v, short target, short step);
extern "C" s16 Vec3_HorzAngle(const Vector3* v0, const Vector3* v1);
extern "C" Fix12 Vec3_Dist(const Vector3* a, const Vector3* b);
extern "C" void func_ov102_0214b988(void* c);
extern "C" void func_ov102_0214beb4(void* c);
struct Animation { void Advance(); };

struct Obj {
    char p0[0x5c];
    Vector3 f5c;          /* 0x5c */
    char g68[0x8e - 0x68];
    short f8e;            /* 0x8e */
    char g90[0x94 - 0x90];
    short f94;            /* 0x94 */
    char g96[0x98 - 0x96];
    int f98;              /* 0x98 */
    char ga0[0xa8 - 0x9c];
    int fa8;              /* 0xa8 */
    char gac[0x350 - 0xac];
    Animation f350;       /* 0x350 */
    char g354[0x35c - 0x354];
    int f35c;             /* 0x35c */
    char g360[0x38c - 0x360];
    void* f38c;           /* 0x38c */
    char g390[0x3c4 - 0x390];
    Vector3 f3c4;         /* 0x3c4 */
    char g3d0[0x3dc - 0x3d0];
    int f3dc;             /* 0x3dc */
    char g3e0[0x3e8 - 0x3e0];
    unsigned short f3e8;  /* 0x3e8 */
    char g3ea[0x3f5 - 0x3ea];
    unsigned char f3f5;   /* 0x3f5 */
};

extern "C" void func_ov102_0214bf64(Obj* o)
{
    DecIfAbove0_Short(&o->f3e8);
    if (o->f3f5 != 1) {
        if (o->f38c == 0) {
            _Z14ApproachLinearRiii(o->f98, 0x5000, 0x2aa);
            func_ov102_0214b988(o);
            if (o->f38c != 0) {
                o->f98 = 0;
                o->fa8 = 0x14000;
                o->f3dc = 2;
            } else {
                if (o->f3f5 == 5) {
                    s16 ang = Vec3_HorzAngle(&o->f5c, &o->f3c4);
                    _Z14ApproachLinearRsss(o->f94, ang, 0x200);
                    o->f8e = o->f94;
                } else {
                    if (o->f3e8 == 0 || Vec3_Dist(&o->f5c, &o->f3c4) >= 0x500000) {
                        o->f98 = 0x5000;
                        func_ov102_0214beb4(o);
                    }
                }
            }
        } else {
            _Z14ApproachLinearRiii(o->f98, 0x10000, 0x1000);
            s16 target = Vec3_HorzAngle(&o->f5c, (Vector3*)((char*)o->f38c + 0x5c));
            if (o->f3f5 == 3) {
                int b = (*(int*)((char*)o->f38c + 0x35c) != 0);
                if (b)
                    target = o->f94;
            }
            _Z14ApproachLinearRsss(o->f94, target, 0x800);
            o->f8e = o->f94;
        }
        o->f35c = o->f98 >> 3;
    }
    o->f350.Advance();
}
