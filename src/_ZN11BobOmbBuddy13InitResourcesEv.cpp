//cpp
typedef int Fix12;
struct Vector3 { int x, y, z; };
struct RaycastGround { char buf0[0x14]; int floor[12]; char buf1[0x50-0x14-0x30]; };

extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* file, int a, int b);
extern void _ZN11ShadowModel12InitCylinderEv(void* self);
extern void* _ZN9Animation8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* self, void* actor, Fix12 b, Fix12 c, unsigned int d, unsigned int e);
extern void func_ov084_0212c960(void* c, int i);
extern void _ZN13RaycastGroundC1Ev(RaycastGround* self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround* self, const Vector3& v, void* actor);
extern int _ZN13RaycastGround10DetectClsnEv(RaycastGround* self);
extern int func_ov084_0212ca60(void* p);
extern void* _ZN5Actor13ClosestPlayerEv(void* self);
extern int _ZN8SaveData16HasPlayerLostCapEv(void);
extern int func_ov084_0212cac0(char* c);
extern int SublevelToLevel(int i);
extern int IsStarCollected(int r0, int r1);
extern void _ZN13RaycastGroundD1Ev(RaycastGround* self);

extern void* data_ov084_02130da4;
extern void* data_ov084_02130d9c;
extern signed char data_0209f2f8;
extern unsigned char data_0209f220;
}

extern "C" int _ZN11BobOmbBuddy13InitResourcesEv(char* self) {
    RaycastGround rc;
    Vector3 pos;

    _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0x108,
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov084_02130da4), 1, -1);
    _ZN11ShadowModel12InitCylinderEv(self + 0x16c);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov084_02130d9c);
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(self + 0xd4, self, 0x8c000, 0x8c000, 0x4200004, 0);
    func_ov084_0212c960(self, 0);
    *(int*)(self + 0x198) = 0;

    {
        int z = *(int*)(self + 0x64);
        int y = *(int*)(self + 0x60) + 0x64000;
        pos.x = *(int*)(self + 0x5c);
        pos.y = y;
        pos.z = z;
    }

    _ZN13RaycastGroundC1Ev(&rc);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rc, pos, 0);
    if (_ZN13RaycastGround10DetectClsnEv(&rc) != 0)
        *(int*)(self + 0x60) = rc.floor[(0x44 - 0x14) / 4];

    if (func_ov084_0212ca60(self) != 0) {
        void* player = _ZN5Actor13ClosestPlayerEv(self);
        unsigned char state = *(unsigned char*)((char*)player + 0x6d9);

        if (state != 0) goto state_check1;
        if (*(int*)(self + 8) == 0xb26)
            goto state_return0;

    state_check1:
        if (state != 1) goto state_check2;
        if (*(int*)(self + 8) == 0xb27)
            goto state_return0;

    state_check2:
        if (state != 2) goto state_lostcap;
        if (*(int*)(self + 8) == 0xb28)
            goto state_return0;

    state_lostcap:
        if (!_ZN8SaveData16HasPlayerLostCapEv())
            goto after_lostcap;

    state_return0:
        _ZN13RaycastGroundD1Ev(&rc);
        return 0;
    }

after_lostcap:
    if (func_ov084_0212cac0(self) == 0)
        goto return1;
    if (data_0209f2f8 != 8)
        goto return1;
    if (data_0209f220 == 1)
        goto return0_2;
    if (IsStarCollected(SublevelToLevel(8), 1) != 0)
        goto return1;

return0_2:
    _ZN13RaycastGroundD1Ev(&rc);
    return 0;

return1:
    _ZN13RaycastGroundD1Ev(&rc);
    return 1;
}
