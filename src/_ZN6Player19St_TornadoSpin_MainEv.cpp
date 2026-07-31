//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef long long s64;

struct Vector3 { int x, y, z; };
struct RaycastGround { int filler[21]; };

extern "C" {
extern void _ZN6Player11ChangeStateERNS_5StateE(char* self, int state);
extern void _ZN13RaycastGroundC1Ev(struct RaycastGround* self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(struct RaycastGround* self, const struct Vector3* pos, char* actor);
extern int _ZN13RaycastGround10DetectClsnEv(struct RaycastGround* self);
extern void Player_AdvanceAnims(char* self);
extern void _ZN13RaycastGroundD1Ev(struct RaycastGround* self);
extern int data_ov002_02110454[];
extern short data_02082214[];

int _ZN6Player19St_TornadoSpin_MainEv(char* c)
{
    struct Vector3 pos;
    struct Vector3 clsnPos;
    struct RaycastGround rc;
    int dx = *(int*)(c + 0x5c) - *(int*)(*(char**)(c + 0x364) + 0x5c);
    int dz = *(int*)(c + 0x64) - *(int*)(*(char**)(c + 0x364) + 0x64);
    int sx = (int)((dx * 0xf33LL + 0x800) >> 12);
    int sz = (int)((dz * 0xf33LL + 0x800) >> 12);
    int spd = *(int*)(c + 0xa8);
    if (spd < 0) {
        *(int*)(c + 0xa8) = 0x1000;
    } else if (spd < 0x3c000) {
        *(int*)(((s64)(int)(c + 0xa8)) & 0xFFFFFFFFFFFFFFFFLL) += 0x1000;
    }
    *(int*)(((s64)(int)(c + 0x688)) & 0xFFFFFFFFFFFFFFFFLL) += *(int*)(c + 0xa8);
    if (*(int*)(c + 0x688) < 0)
        *(int*)(c + 0x688) = 0;
    if (*(int*)(c + 0x688) > *(int*)(*(char**)(c + 0x364) + 0xdc) || (*(u8*)(c + 0x6e9) & 4) != 0) {
        *(u8*)(c + 0x6e6) = 0;
        _ZN6Player11ChangeStateERNS_5StateE(c, (int)data_ov002_02110454);
        if (*(int*)(c + 0xa8) < 0x14000)
            *(int*)(c + 0xa8) = 0x14000;
        return 1;
    }
    if (*(s16*)(c + 0x69c) < 0x3000)
        *(s16*)(((s64)(int)(c + 0x69c)) & 0xFFFFFFFFFFFFFFFFLL) += 0x100;
    *(s16*)(((s64)(int)(c + 0x8e)) & 0xFFFFFFFFFFFFFFFFLL) += *(s16*)(c + 0x69c);
    {
        int v = *(s16*)(c + 0x69c);
        if (v >= 0x1000) v = 0x1000;
        {
            int k = (u16)v >> 4;
            int sinv = data_02082214[k * 2 + 1];
            int cosv = data_02082214[k * 2];
            ((int*)&pos)[0] = (int)(((s64)sz * cosv + 0x800) >> 12) + (*(int*)(*(char**)(c + 0x364) + 0x5c) + (int)(((s64)sx * sinv + 0x800) >> 12));
            ((int*)&pos)[2] = (int)(((s64)sz * sinv + 0x800) >> 12) + (*(int*)(*(char**)(c + 0x364) + 0x64) - (int)(((s64)sx * cosv + 0x800) >> 12));
            ((int*)&pos)[1] = *(int*)(c + 0x688) + *(int*)(*(char**)(c + 0x364) + 0x60);
        }
    }
    _ZN13RaycastGroundC1Ev(&rc);
    clsnPos = pos;
    clsnPos.y += 0x64000;
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rc, &clsnPos, c);
    if (_ZN13RaycastGround10DetectClsnEv(&rc) != 0) {
        *(int*)(c + 0x5c) = pos.x;
        *(int*)(c + 0x60) = pos.y;
        *(int*)(c + 0x64) = pos.z;
    } else {
        if (*(int*)(c + 0x60) >= *(int*)(c + 0x644))
            *(int*)(c + 0x60) = pos.y;
        else
            *(int*)(c + 0x60) = *(int*)(c + 0x644);
    }
    Player_AdvanceAnims(c);
    _ZN13RaycastGroundD1Ev(&rc);
    return 1;
}
}
