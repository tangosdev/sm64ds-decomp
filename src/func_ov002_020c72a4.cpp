//cpp
struct Vector3 { int x, y, z; };
struct Actor;
struct RaycastGround {
    char pad0[0x14];
    int f14;
    char pad1[0x38];
    RaycastGround();
    ~RaycastGround();
    void SetObjAndPos(const Vector3&, Actor*);
    int DetectClsn();
};
extern "C" void _ZN4BgCh19StartDetectingWaterEv(RaycastGround*);
extern "C" int func_02037e78(int* p);
extern "C" void func_ov002_020c71e0(void*);
extern int STAR_CAP_MAX_POS_Y;
extern int WATER_HEIGHT;

extern "C" void func_ov002_020c72a4(void* thisptr)
{
    unsigned char* r4 = (unsigned char*)thisptr;
    RaycastGround rg;
    Vector3 v;
    int z = *(int*)(r4 + 0x64);
    int d = STAR_CAP_MAX_POS_Y;
    int x = *(int*)(r4 + 0x5c);
    v.x = x;
    v.y = d;
    v.z = z;
    *(int*)((char*)&rg + 0x4c) = d << 1;
    rg.SetObjAndPos(v, (Actor*)thisptr);
    _ZN4BgCh19StartDetectingWaterEv(&rg);
    if (rg.DetectClsn()) {
        if (func_02037e78(&rg.f14) != 0) {
            *(int*)(r4 + 0x64c) = *(int*)((char*)&rg + 0x44);
            WATER_HEIGHT = *(int*)(r4 + 0x64c);
        }
    }
    *(int*)((char*)&rg + 0x4c) = 0x1f4000;
    func_ov002_020c71e0(thisptr);
}
