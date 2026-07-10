//cpp
struct Vector3 { int x, y, z; };
struct RaycastGround { char buf0[0x14]; int floor[12]; char buf1[0x50-0x14-0x30]; };

extern "C" void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* file, int a, int b);
extern "C" void _ZN11ShadowModel10InitCuboidEv(void* self);
extern "C" void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* self, void* actor, int a, int b, unsigned int c, unsigned int d);
extern "C" void _ZN13RaycastGroundC1Ev(RaycastGround* self);
extern "C" void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround* self, const Vector3& v, void* actor);
extern "C" int _ZN13RaycastGround10DetectClsnEv(RaycastGround* self);
extern "C" void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* self, void* actor, int a, int b, void* v, int c);
extern "C" void func_ov080_0212513c(void* self);
extern "C" void func_ov080_02124c3c(void* self);
extern "C" void _ZN13RaycastGroundD1Ev(RaycastGround* self);

extern void* data_ov080_02128468;

extern "C" int _ZN11CrazedCrate13InitResourcesEv(char* self) {
    RaycastGround rc;
    Vector3 pos;
    void* file = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov080_02128468);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0xd4, file, 1, 1);
    _ZN11ShadowModel10InitCuboidEv(self + 0x124);
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(self + 0x14c, self, 0x64000, 0x78000, 0x800004, 0x9010);
    *(int*)(self + 0x9c) = -0x2000;
    *(int*)(self + 0xa0) = -0x3c000;
    {
        int p60;
        pos.x = *(int*)(self + 0x5c);
        p60 = *(int*)(self + 0x60);
        pos.y = p60;
        pos.z = *(int*)(self + 0x64);
        pos.y = p60 + 0xc8000;
    }
    _ZN13RaycastGroundC1Ev(&rc);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rc, pos, 0);
    if (_ZN13RaycastGround10DetectClsnEv(&rc))
        *(int*)(self + 0x60) = rc.floor[(0x44 - 0x14) / 4];
    else
        *(int*)(self + 0x60) = pos.y;
    *(int*)(self + 0x80) = 0x1000;
    *(int*)(self + 0x84) = 0x1000;
    *(int*)(self + 0x88) = 0x1000;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(self + 0x180, self, 0x32000, 0x32000, 0, 0);
    *(int*)(self + 0x374) = 0;
    func_ov080_0212513c(self);
    func_ov080_02124c3c(self);
    _ZN13RaycastGroundD1Ev(&rc);
    return 1;
}
