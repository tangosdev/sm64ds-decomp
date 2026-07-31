typedef unsigned int u32;
typedef short s16;
typedef signed char s8;

typedef struct Vector3 { int x, y, z; } Vector3;

typedef struct RaycastLine { int _[0x1e]; } RaycastLine;
typedef struct RaycastGround { int _[0x15]; } RaycastGround;

extern Vector3 data_02086f2c;
extern Vector3 data_02086f20;

extern u32 func_020093f4(void* p, int x);
extern u32 func_020093d4(void* p, int a);
extern void Vec3_RotateYAndTranslate(Vector3* out, Vector3* in, s16 angle, Vector3* src);
extern void _ZN11RaycastLineC1Ev(RaycastLine* t);
extern void _ZN11RaycastLineD1Ev(RaycastLine* t);
extern void func_0200897c(void* self, void* arg);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(RaycastLine* t, Vector3* a, Vector3* b, void* actor);
extern int _ZN11RaycastLine10DetectClsnEv(RaycastLine* t);
extern void _ZN11RaycastLine10GetClsnPosEv(Vector3* ret, RaycastLine* t);
extern void _ZN13RaycastGroundC1Ev(RaycastGround* t);
extern void _ZN13RaycastGroundD1Ev(RaycastGround* t);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround* t, Vector3* a, void* actor);
extern int _ZN13RaycastGround10DetectClsnEv(RaycastGround* t);
extern void func_0200928c(void* cam);
extern void ChangeArea(int areaID);

void func_0200cce4(char* cam)
{
    Vector3 vec;
    Vector3 tmp1;
    Vector3 tmp2;
    Vector3 cp;
    RaycastLine line;
    RaycastGround ground;
    u32 d;

    if (*(char**)(cam + 0x110) != 0) {
        Vector3* p = (Vector3*)(((int)*(char**)(cam + 0x110) + 0x5c));
        *(int*)(cam + 0x98) = p->x;
        *(int*)(cam + 0x9c) = p->y;
        *(int*)(cam + 0xa0) = p->z;

        {
            u32 d1 = func_020093f4(cam, *(int*)(*(char**)(cam + 0x13c) + 0x14));
            tmp1.z = d1;
            tmp1.x = data_02086f2c.x;
            tmp1.y = data_02086f2c.y;
            data_02086f2c.z = d1;
        }
        Vec3_RotateYAndTranslate((Vector3*)(cam + 0x80), (Vector3*)(cam + 0x98),
            *(s16*)(*(char**)(cam + 0x110) + 0x8e), &tmp1);

        d = func_020093d4(cam, *(int*)(*(char**)(cam + 0x13c) + 0x10));
        *(int*)(((int)cam + 0x84)) += d;

        {
            u32 d2 = func_020093f4(cam, -*(int*)(*(char**)(cam + 0x13c) + 0x20));
            tmp2.z = d2;
            data_02086f20.z = d2;
            tmp2.x = data_02086f20.x;
            tmp2.y = data_02086f20.y;
        }
        Vec3_RotateYAndTranslate((Vector3*)(cam + 0x8c), (Vector3*)(cam + 0x80),
            *(s16*)(*(char**)(cam + 0x110) + 0x8e), &tmp2);

        _ZN11RaycastLineC1Ev(&line);
        func_0200897c(cam, &line);
        {
            int tx;
            int ty;
            int tz;
            tx = *(int*)(cam + 0x8c);
            tz = *(int*)(cam + 0x94);
            ty = *(int*)(cam + 0x90) + 0x100000;
            vec.x = tx;
            vec.y = ty;
            vec.z = tz;
        }
        _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(&line, (Vector3*)(cam + 0x8c), &vec, 0);
        if (_ZN11RaycastLine10DetectClsnEv(&line) != 0) {
            _ZN11RaycastLine10GetClsnPosEv(&cp, &line);
            vec.x = cp.x;
            vec.y = cp.y;
            vec.z = cp.z;
        }
        _ZN13RaycastGroundC1Ev(&ground);
        func_0200897c(cam, &ground);
        _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&ground, &vec, 0);
        if (_ZN13RaycastGround10DetectClsnEv(&ground) != 0) {
            *(int*)(cam + 0x90) = d + ground._[0x11];
        }
        *(int*)(((int)cam + 0x90)) += func_020093d4(cam, 0x3c286);
        {
            int diff;
            int base;
            int a;
            base = *(int*)(cam + 0x84);
            diff = *(int*)(cam + 0x90) - base;
            a = (diff < 0) ? -diff : diff;
            if (a > 0x3e8000) {
                if (diff < -0x3e8000) diff = -0x3e8000;
                else if (diff > 0x3e8000) diff = 0x3e8000;
                *(int*)(cam + 0x90) = base + diff;
            }
        }
        {
            u32 t = func_020093d4(cam, *(int*)(*(char**)(cam + 0x13c) + 0x10));
            *(int*)(cam + 0xa4) = 0;
            *(int*)(cam + 0xa8) = t;
            *(int*)(cam + 0xac) = 0;
        }
        func_0200928c(cam);
        ChangeArea(*(s8*)(*(char**)(cam + 0x110) + 0xcc));
        _ZN13RaycastGroundD1Ev(&ground);
        _ZN11RaycastLineD1Ev(&line);
    }
    *(int*)(((int)cam + 0x154)) |= 4;
}
