//cpp
struct Vector3 { int x, y, z; };
struct RaycastGround { char buf0[0x14]; int floor[12]; char buf1[0x50-0x14-0x30]; };

extern "C" void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* file, int a, int b);
extern "C" int _ZN11ShadowModel12InitCylinderEv(void* self);
extern "C" void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* self, void* actor, int a, int b, unsigned int c, unsigned int d);
extern "C" void _ZN13RaycastGroundC1Ev(RaycastGround* self);
extern "C" void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround* self, const Vector3& v, void* actor);
extern "C" int _ZN13RaycastGround10DetectClsnEv(RaycastGround* self);
extern "C" void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* self, void* actor, int a, int b, void* v, int c);
extern "C" void func_ov072_0211fcb0(char* self, int a);
extern "C" void func_ov072_0211f3e4(char* self);
extern "C" void _ZN7PathPtr6FromIDEj(void* self, unsigned int id);
extern "C" void _ZN13RaycastGroundD1Ev(RaycastGround* self);

struct Block48 { int w[12]; };

extern void* data_ov072_02122b20;
extern Block48 data_02082128;

extern "C" int func_ov072_0211fd54(char* self) {
    RaycastGround rc;
    Vector3 pos;
    void* file = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov072_02122b20);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0xd4, file, 1, 1);
    if (_ZN11ShadowModel12InitCylinderEv(self + 0x124) == 0)
        return 0;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(self + 0x14c, self, 0x82000, 0x104000, 0x800004, 0);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(self + 0x180, self, 0x82000, 0x82000, 0, 0);
    {
        int p60;
        pos.x = *(int*)(self + 0x5c);
        p60 = *(int*)(self + 0x60);
        pos.y = p60;
        pos.z = *(int*)(self + 0x64);
        pos.y = p60 + 0x14000;
    }
    _ZN13RaycastGroundC1Ev(&rc);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rc, pos, 0);
    if (_ZN13RaycastGround10DetectClsnEv(&rc))
        *(int*)(self + 0x60) = rc.floor[(0x44 - 0x14) / 4];
    else
        *(int*)(self + 0x60) = pos.y;
    *(int*)(self + 0x33c) = *(int*)(self + 0x5c);
    *(int*)(self + 0x340) = *(int*)(self + 0x60);
    *(int*)(self + 0x344) = *(int*)(self + 0x64);
    *(short*)(self + 0x348) = *(short*)(self + 0x8c);
    *(short*)(self + 0x34a) = *(short*)(self + 0x8e);
    *(short*)(self + 0x34c) = *(short*)(self + 0x90);
    *(unsigned char*)(self + 0x3a4) = 0x5a;
    func_ov072_0211fcb0(self, 0);
    *(int*)(self + 0x390) = 0;
    *(Block48*)(self + 0x350) = data_02082128;
    func_ov072_0211f3e4(self);
    _ZN7PathPtr6FromIDEj(self + 0x380, *(unsigned int*)(self + 8) & 0xff);
    _ZN13RaycastGroundD1Ev(&rc);
    return 1;
}
