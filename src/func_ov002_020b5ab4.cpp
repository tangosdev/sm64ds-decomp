//cpp
typedef int s32;
typedef signed char s8;
struct Vector3 { s32 x, y, z; };
struct Actor;

struct RaycastGround {
    char pad0[0x14];
    int field14;     /* 0x14 */
    char pad18[0x44 - 0x18];
    int field44;     /* 0x44 */
    char pad48[0x54 - 0x48];
    RaycastGround();
    ~RaycastGround();
    void StartDetectingWater();
    void SetObjAndPos(const Vector3& pos, Actor* a);
    int DetectClsn();
};

extern "C" int func_02037e78(int* p);
extern "C" s8 LEVEL_ID;
extern "C" int WATER_HEIGHT;

extern "C" int func_ov002_020b5ab4(char* c);
int func_ov002_020b5ab4(char* c)
{
    if (LEVEL_ID == 0x15) {
        int x = *(int*)(c+0x344);
        int y;
        *(int*)(c+0x324) = x;
        y = WATER_HEIGHT;
        if (y > x) {
            *(int*)(c+0x324) = y;
            return 1;
        }
        return 0;
    }
    {
        Vector3 vec;
        RaycastGround rg;
        {
            int vx = *(int*)(c+0x5c);
            int vz = *(int*)(c+0x64);
            int vy = *(int*)(c+0x60) + 0x64000;
            vec.x = vx;
            vec.y = vy;
            vec.z = vz;
        }
        rg.StartDetectingWater();
        rg.SetObjAndPos(vec, (Actor*)c);
        if (rg.DetectClsn() != 0) {
            *(int*)(c+0x324) = rg.field44;
            if (func_02037e78(&rg.field14) != 0) {
                return 1;
            }
        }
    }
    return 0;
}
