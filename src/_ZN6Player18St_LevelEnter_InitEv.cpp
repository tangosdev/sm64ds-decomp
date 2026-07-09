//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef int s32;
typedef long long s64;

struct Vector3 { int x, y, z; };

typedef struct RaycastGround {
    char pre[0x10];
    char result[0x34];
    int clsnY;
    char post[0x8];
} RaycastGround;

extern "C" {
int _ZN8SaveData16HasPlayerLostCapEv(void);
void _ZN6Player7SetAnimEji5Fix12IiEj(void* self, u32 anim, int a, int fix, u32 b);
int _ZNK6Player14GetBodyModelIDEjb(void* self, u32 a, int b);
void _ZN9Animation8SetFlagsEi(void* self, int flags);
void func_ov002_020c9e40(char* self);
void _ZN13RaycastGroundC1Ev(RaycastGround* self);
void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround* self, Vector3* pos, void* actor);
int _ZN13RaycastGround10DetectClsnEv(RaycastGround* self);
int func_ov002_020c7cbc(char* self);
void func_02012790(int a);
void _ZN6Player16InitWingFeathersEb(void* self, int b);
void _ZN13RaycastGroundD1Ev(RaycastGround* self);
}

extern u32 data_ov002_0210a7e8[];
extern u8 data_0209f2d8;
extern s16 data_02082214[];
extern signed char data_0209f2f8;
extern u8 data_0209f264;
extern u8 data_0209f2fc;

extern "C" int _ZN6Player18St_LevelEnter_InitEv(char* self)
{
    RaycastGround rg;

    u32 anim = data_ov002_0210a7e8[*(u8*)(self + 0x6e3)];
    if (_ZN8SaveData16HasPlayerLostCapEv() != 0) {
        if (anim == 0x85) anim = 0xa5;
    }
    _ZN6Player7SetAnimEji5Fix12IiEj(self, anim, 0, 0x1000, 0);

    u8 st = *(u8*)(self + 0x6e3);
    if (st != 3 && st != 0xa && st != 0) {
        _ZN9Animation8SetFlagsEi(
            *(char**)((self + (_ZNK6Player14GetBodyModelIDEjb(self, (u8)*(int*)(self + 8), 0) * 4)) + 0xdc) + 0x50,
            0x40000000);
        if (*(u8*)(self + 0x6e3) == 2) {
            char* p = *(char**)((self + (_ZNK6Player14GetBodyModelIDEjb(self, (u8)*(int*)(self + 8), 0) * 4)) + 0xdc) + 0x50;
            *(int*)(p + 8) = 0x14000;
        }
    }

    *(signed char*)(self + 0x719) = -1;
    func_ov002_020c9e40(self);
    *(u8*)(self + 0x6e1) = 0;
    *(int*)(self + 0x98) = 0;
    *(u8*)(self + 0x70c) = 0;
    *(s16*)(self + 0x94) = *(s16*)(self + 0x8e);
    *(u8*)(self + 0x6e5) = 0;
    _ZN13RaycastGroundC1Ev(&rg);

    switch (*(u8*)(self + 0x6e3)) {
    case 0: {
        Vector3 pos;
        int hit;
        int y = *(int*)(self + 0x60);
        int z = *(int*)(self + 0x64);
        int x = *(int*)(self + 0x5c);
        int w = y + 0xc8000;
        pos.x = x;
        pos.y = w;
        pos.z = z;
        _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, &pos, self);
        hit = 0x80000000;
        if (_ZN13RaycastGround10DetectClsnEv(&rg) != 0) hit = rg.clsnY;
        *(int*)(self + 0x60) = hit;
        *(u8*)(self + 0x6e5) = 2;
        {
            int b = (data_0209f2d8 == 2);
            if (b == 0) *(s16*)(self + 0x6a6) = 0x1e;
            else *(s16*)(self + 0x6a6) = 0;
        }
        break;
    }
    case 1:
        *(int*)(self + 0x9c) = 0;
        *(u8*)(self + 0x6e5) = 4;
        *(u8*)(self + 0x706) = 1;
        break;
    case 5:
    case 15:
        *(u8*)(self + 0x70c) = 1;
        break;
    case 11:
    case 12: {
        s32 idx;
        s16 cosv, sinv;
        if (func_ov002_020c7cbc(self) != 0) *(u8*)(self + 0x6e3) = 0x12;
        *(int*)(((long long)(int)(self + 0x60)) & 0xFFFFFFFFFFFFFFFFLL) += 0x64000;
        idx = ((s32)(u16)(s16)(*(s16*)(self + 0x8e) + 0x8000) >> 4) * 2;
        cosv = data_02082214[idx];
        sinv = data_02082214[idx + 1];
        *(int*)(((long long)(int)(self + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL) += (int)(((s64)0xa0000 * cosv + 0x800) >> 12);
        *(int*)(((long long)(int)(self + 0x64)) & 0xFFFFFFFFFFFFFFFFLL) += (int)(((s64)0xa0000 * sinv + 0x800) >> 12);
        *(s16*)(self + 0x6a4) = 0x20;
        *(u8*)(self + 0x70c) = 1;
        *(int*)(self + 0x9c) = 0;
        *(u8*)(self + 0x6e5) = 5;
        break;
    }
    case 8:
    case 9:
        if (func_ov002_020c7cbc(self) != 0) *(u8*)(self + 0x6e3) = 0x10;
        *(s16*)(self + 0x6a4) = 2;
        *(u8*)(self + 0x70c) = 1;
        *(int*)(self + 0x9c) = 0;
        *(u8*)(self + 0x6e5) = 5;
        break;
    case 13:
    case 17: {
        u8 st2 = *(u8*)(self + 0x6e3);
        if (st2 == 0x11) *(s16*)(self + 0x6a4) = 2;
        else *(s16*)(self + 0x6a4) = 0x20;
        *(u8*)(self + 0x70c) = 1;
        *(int*)(self + 0x9c) = 0;
        *(u8*)(self + 0x6e5) = 5;
        if (data_0209f2f8 == 0x18) func_02012790(0x16);
        else if (data_0209f2f8 == 0x19) func_02012790(0x17);
        break;
    }
    case 2:
        if (*(int*)(self + 8) != 0) {
            *(u8*)(self + 0x6e3) = 0xa;
            _ZN6Player7SetAnimEji5Fix12IiEj(self, data_ov002_0210a7e8[*(u8*)(self + 0x6e3)], 0, 0x1000, 0);
        } else {
            _ZN6Player16InitWingFeathersEb(self, 0);
            *(u8*)(self + 0x6e5) = 3;
            *(int*)(self + 0xa8) = 0;
            *(int*)(self + 0x9c) = 0;
        }
        break;
    default:
        break;
    }

    *(u8*)(self + 0x6de) = 1;
    *(u8*)(self + 0x6df) = 0;
    {
        u8 c2 = data_0209f264;
        if (data_0209f2f8 == 2) {
            if (c2 == 0 || (c2 == 0xe && data_0209f2fc != 1)) {
                func_02012790(0x1d);
            }
        } else if (data_0209f2f8 == 1 && (c2 == 6 || c2 == 7 || (u32)(c2 - 9) <= 1)) {
            func_02012790(0x1d);
        }
    }

    _ZN13RaycastGroundD1Ev(&rg);
    return 1;
}
