//cpp
struct Vector3 { int x, y, z; };
struct RaycastGround { char buf0[0x14]; int floor[12]; char buf1[0x50-0x14-0x30]; };

struct Actor { };
typedef void (Actor::*ActorFn)();

extern "C" int _ZN11ShadowModel12InitCylinderEv(void* self);
extern "C" void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* self, void* actor, int a, int b, unsigned int c, unsigned int d);
extern "C" void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* self, void* actor, int a, int b, void* v, int c);
extern "C" void _ZN13RaycastGroundC1Ev(RaycastGround* self);
extern "C" void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround* self, const Vector3& v, void* actor);
extern "C" int _ZN13RaycastGround10DetectClsnEv(RaycastGround* self);
extern "C" void _ZN13RaycastGroundD1Ev(RaycastGround* self);

extern ActorFn data_ov060_0211af74[];

extern "C" int _ZN10BowserFire13InitResourcesEv(char* c) {
    RaycastGround rc;
    Vector3 pos;
    if (_ZN11ShadowModel12InitCylinderEv(c + 0x304) == 0)
        return 0;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x2d0, c, 0x28000, 0x50000, 0x200002, 0);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x110, c, 0x32000, 0x32000, 0, 0);
    *(int*)(c + 0x9c) = -0x4000;
    *(int*)(c + 0xa0) = -0x1e000;
    *(int*)(c + 0x35c) = *(int*)(c + 8) & 7;
    *(short*)(c + 0x374) = 0;
    if (*(int*)(c + 0x35c) == 0)
        *(unsigned char*)(c + 0x379) = 0;
    else
        *(unsigned char*)(c + 0x379) = 1;
    *(int*)(c + 0x36c) = 0;
    *(unsigned char*)(c + 0x378) = (*(unsigned int*)(c + 8) >> 4) & 3;
    if (*(int*)(c + 0x35c) == 0)
        *(int*)(((int)c + 0x2e8) & 0xFFFFFFFFFFFFFFFF) |= 1;
    *(int*)(c + 0x360) = 0x2000;
    *(int*)(c + 0x380) = 0;
    *(int*)(c + 0x37c) = *(int*)(c + 0x380);
    *(int*)(c + 0x2cc) = 0;
    _ZN13RaycastGroundC1Ev(&rc);
    {
        int p60;
        pos.x = *(int*)(c + 0x5c);
        p60 = *(int*)(c + 0x60);
        pos.y = p60;
        pos.z = *(int*)(c + 0x64);
        pos.y = p60 + 0x32000;
    }
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rc, pos, 0);
    if (_ZN13RaycastGround10DetectClsnEv(&rc))
        *(int*)(c + 0x364) = rc.floor[12];
    else
        *(int*)(c + 0x364) = *(int*)(c + 0x60);
    (((Actor*)c)->*data_ov060_0211af74[*(int*)(c + 0x35c)])();
    *(int*)(c + 0x384) = 0;
    *(int*)(c + 0x388) = 0;
    _ZN13RaycastGroundD1Ev(&rc);
    return 1;
}
