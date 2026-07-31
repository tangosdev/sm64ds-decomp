typedef struct Vector3 { int x, y, z; } Vector3;
typedef struct RaycastGround { char filler[0x44]; int clsnY; char rest[0x8]; } RaycastGround;

extern void _ZN13RaycastGroundC1Ev(RaycastGround*);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround*, Vector3*, void*);
extern int _ZN13RaycastGround10DetectClsnEv(RaycastGround*);
extern void _ZN13RaycastGroundD1Ev(RaycastGround*);
extern int _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int, int, int, int);

int func_0200fa04(int a, Vector3* pos, int flag)
{
    RaycastGround rc;
    if (flag) {
        _ZN13RaycastGroundC1Ev(&rc);
        *(int*)(((int)pos + 4)) += 0x32000;
        _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rc, pos, 0);
        if (_ZN13RaycastGround10DetectClsnEv(&rc))
            pos->y = rc.clsnY;
        _ZN13RaycastGroundD1Ev(&rc);
    }
    *(int*)(((int)pos + 4)) += 0x78000;
    return _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x1b, pos->x, pos->y, pos->z);
}
