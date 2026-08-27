//cpp
typedef int s16;
struct Vector3 { int x, y, z; };

struct C;
typedef void (C::*PMF)();

struct C {
    char pad0[0xb4];
    PMF pmf;        // 0xb4 (2 words)
    char pad1[0xc8 - 0xb4 - 8];
    Vector3 v0c8;   // 0xc8
    char pad2[0xea - 0xc8 - 12];
    short a0ea;     // 0xea
    char pad3[0xf0 - 0xea - 2];
    short a0f0;     // 0xf0
    char pad4[0xf4 - 0xf0 - 2];
    int a0f4;       // 0xf4
};

extern "C" void _ZN14BlendModelAnim7AdvanceEv(void*);
extern "C" s16 Vec3_HorzAngle(const Vector3*, const Vector3*);
extern "C" void _Z11UpdateAngleRssis(short*, short, int, short);

extern Vector3 data_ov006_0212b890;
extern "C" int data_ov006_0213ac78[2];

extern "C" void func_ov006_020c07e8(C* c)
{
    (c->*(c->pmf))();

    _ZN14BlendModelAnim7AdvanceEv((char*)c + 0x18);

    if (c->a0f4 != 0) {
        Vector3 t;
        t.x = data_ov006_0212b890.x;
        t.y = data_ov006_0212b890.y;
        t.z = data_ov006_0212b890.z;
        short d;
        int ang;
        ang = Vec3_HorzAngle(&c->v0c8, &t);
        d = (short)(ang - c->a0ea);
        if (d < -0x3000) d = -0x3000;
        else if (d > 0x3000) d = 0x3000;
        _Z11UpdateAngleRssis(&c->a0f0, d, 8, 0x200);

        {
            int* p = (int*)(((int)c + 0xb4));
            int* g = data_ov006_0213ac78;
            if (p[0] == g[0]) {
                if (p[1] == g[1])
                    return;
                if (*(int*)((char*)c + 0xb4) == 0)
                    return;
            }
        }
        d = (short)(ang - d);
        _Z11UpdateAngleRssis(&c->a0ea, d, 8, 0x200);
    } else {
        _Z11UpdateAngleRssis(&c->a0f0, 0, 8, 0x200);
    }
}
