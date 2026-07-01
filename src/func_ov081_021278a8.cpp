//cpp
struct Vector3 { int x, y, z; };
struct RaycastGround { char buf0[0x14]; int floor[12]; char buf1[0x50-0x14-0x30]; };

extern "C" void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern "C" int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* file, int a, int b);
extern "C" void* _ZN9Animation8LoadFileER13SharedFilePtr(void* fp);
extern "C" int _ZN11ShadowModel12InitCylinderEv(void* self);
extern "C" void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* self, void* actor, int a, int b, unsigned int c, unsigned int d);
extern "C" void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* self, void* actor, int a, int b, void* v, int c);
extern "C" void _ZN12WithMeshClsn19StartDetectingWaterEv(void* self);
extern "C" void func_ov081_0212777c(void* self, int a);
extern "C" void _ZN13RaycastGroundC1Ev(RaycastGround* self);
extern "C" void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround* self, const Vector3& v, void* actor);
extern "C" int _ZN13RaycastGround10DetectClsnEv(RaycastGround* self);
extern "C" void func_ov081_02126a20(char* self);
extern "C" void _ZN13RaycastGroundD1Ev(RaycastGround* self);

struct Block48 { int w[12]; };

extern void* data_ov081_02128ed4;
extern int data_ov002_0210d9b8[];
extern void* data_ov081_021280d8[4];
extern Block48 data_02082128;

extern "C" int func_ov081_021278a8(char* c) {
    RaycastGround rc;
    Vector3 pos;
    void* m = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov081_02128ed4);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, m, 1, 1);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x138, (void*)data_ov002_0210d9b8[1], 1, 1) == 0)
        return 0;
    for (int i = 0; i < 4; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov081_021280d8[i]);
    if (_ZN11ShadowModel12InitCylinderEv(c + 0x188) == 0)
        return 0;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x1b0, c, 0x4b000, 0x73000, 0x200000, 0x6eff0);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x1e4, c, 0x4b000, 0x4b000, 0, 0);
    _ZN12WithMeshClsn19StartDetectingWaterEv(c + 0x1e4);
    func_ov081_0212777c(c, 0);
    *(int*)(c + 0x9c) = -0x2000;
    *(int*)(c + 0xa0) = -0x3c000;
    {
        int p60;
        pos.x = *(int*)(c + 0x5c);
        p60 = *(int*)(c + 0x60);
        pos.y = p60;
        pos.z = *(int*)(c + 0x64);
        pos.y = p60 + 0x14000;
    }
    _ZN13RaycastGroundC1Ev(&rc);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rc, pos, 0);
    if (_ZN13RaycastGround10DetectClsnEv(&rc))
        *(int*)(c + 0x60) = rc.floor[(0x44 - 0x14) / 4];
    else
        *(int*)(c + 0x60) = pos.y;
    *(int*)(c + 0x80) = 0x1000;
    *(int*)(c + 0x84) = 0x1000;
    *(int*)(c + 0x88) = 0x1000;
    *(int*)(c + 0x3d0) = *(int*)(c + 0x5c);
    *(int*)(c + 0x3d4) = *(int*)(c + 0x60);
    *(int*)(c + 0x3d8) = *(int*)(c + 0x64);
    *(unsigned char*)(c + 0x3f0) = 1;
    *(Block48*)(c + 0x3a0) = data_02082128;
    func_ov081_02126a20(c);
    _ZN13RaycastGroundD1Ev(&rc);
    return 1;
}
