//cpp
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int u32;
typedef long long s64;

struct Vector3 { int x, y, z; };

extern "C" {
int _ZN6Player12GetTalkStateEv(void* self);
int _Z14ApproachLinearRsss(short* cur, short target, short step);
void _ZN7Message11PrepareTalkEv(void);
void _ZN6Player12ShowMessage2ER9ActorBasejPK7Vector3jj(void* self, void* actor, unsigned int msg, const void* vec, unsigned int a, unsigned int b);
void func_02012790(int a);
int _ZN6Player18HasFinishedTalkingEv(void* self);
void _ZN7Message7EndTalkEv(void);
void _ZN7Message13DisplaySavingEt(unsigned short a);
void StartMinigameMenu(unsigned char a);
void* _ZN5Actor10FindWithIDEj(unsigned int id);
short Vec3_HorzAngle(const struct Vector3* a, const struct Vector3* b);
int AngleDiff(int a, int b);
int _ZN6Player9StartTalkER9ActorBaseb(void* actor, void* self, int b);
void _ZN12CylinderClsn5ClearEv(void* self);
void _ZN12CylinderClsn6UpdateEv(void* self);
}

extern s16 data_02082214[];
extern u8 data_0209d684;
extern u8 data_0209d660;

#pragma opt_strength_reduction off
extern "C" int func_ov058_0211123c(char* self)
{
    volatile Vector3 v1, v2, v3;
    Vector3 apos;
    volatile Vector3 pos;
    char* target = *(char**)(self + 0x214);

    if (target) {
        v1.x = *(volatile int*)(self + 0x5c);
        v1.y = *(volatile int*)(self + 0x60);
        v1.z = *(volatile int*)(self + 0x64);
        v2.x = *(int*)(self + 0x5c);
        v2.y = *(int*)(self + 0x60);
        v2.z = *(int*)(self + 0x64);
        v2.x = *(int*)(self + 0x5c) + (int)(((s64)0x5a000 * data_02082214[(*(volatile u16*)(self + 0x8e) >> 4) * 2] + 0x800) >> 12);
        v2.z = *(int*)(self + 0x64) + (int)(((s64)0x5a000 * data_02082214[(*(volatile u16*)(self + 0x8e) >> 4) * 2 + 1] + 0x800) >> 12);
        {
            struct Vector3* tp = (struct Vector3*)(((int)target + 0x5c) & 0xffffffffffffffffull);
            v3.x = tp->x;
            v3.y = tp->y;
            v3.z = tp->z;
        }

        switch (_ZN6Player12GetTalkStateEv(target)) {
        case 0: {
            short cur = (short)(*(short*)(self + 0x8e) + 0x8000);
            if (_Z14ApproachLinearRsss((short*)(target + 0x8e), cur, 0x800) != 0) {
                *(u16*)(self + 0x21a) = 0xb09;
                _ZN7Message11PrepareTalkEv();
                _ZN6Player12ShowMessage2ER9ActorBasejPK7Vector3jj(
                    target, self, *(s16*)(self + 0x21a), 0, 1, 0);
            }
            break;
        }
        case 1:
            break;
        default: {
            u8 st = data_0209d684;
            if (*(u16*)(self + 0x21a) == 0xb09) {
                if (st == 1) {
                    if (*(u8*)(self + 0x218) == 0) {
                        func_02012790(0x98);
                        *(u8*)(self + 0x218) = 1;
                    }
                    if (_ZN6Player18HasFinishedTalkingEv(target) != 0) {
                        _ZN7Message7EndTalkEv();
                        *(int*)(self + 0x214) = 0;
                        *(u8*)(self + 0x218) = 0;
                    }
                } else if (st == 2) {
                    if (*(u8*)(self + 0x218) == 0) {
                        func_02012790(0x97);
                        *(u8*)(self + 0x218) = 1;
                    }
                    _ZN7Message13DisplaySavingEt(0x295);
                } else {
                    if (data_0209d660 == 0) {
                        StartMinigameMenu(1);
                        *(int*)(self + 0x214) = 0;
                        *(u8*)(self + 0x218) = 0;
                    }
                }
            } else {
                *(int*)(self + 0x214) = 0;
                *(u8*)(self + 0x218) = 0;
            }
            break;
        }
        }
    } else {
        int i;
        for (i = 0; i < 5; i++) {
            char* entry = self + (i << 6);
            if (*(int*)(entry + 0xf4) & 0x8000000) {
                u32 id = *(u32*)(entry + 0xf8);
                char* actor = (char*)_ZN5Actor10FindWithIDEj(id);
                if (actor) {
                    int isMatch = (*(u16*)(actor + 0xc) == 0xbf);
                    if (isMatch != false) {
                        struct Vector3* ap = (struct Vector3*)(((int)actor + 0x5c) & 0xffffffffffffffffull);
                        short ang;
                        int diff;
                        apos.x = ap->x;
                        apos.y = ap->y;
                        apos.z = ap->z;
                        ang = Vec3_HorzAngle((struct Vector3*)(self + 0x5c), &apos);
                        diff = AngleDiff(ang, *(short*)(self + 0x8e));
                        if (diff < 0x4000) {
                            if (_ZN6Player9StartTalkER9ActorBaseb(actor, self, 0) != 0) {
                                *(char**)(self + 0x214) = actor;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    {
        int j;
        char* cyl = self + 0xd4;
        for (j = 0; j < 5; j++) {
            pos.x = *(int*)(self + 0x5c);
            pos.y = *(int*)(self + 0x60);
            pos.z = *(int*)(self + 0x64);
            {
                int distFixed = ((2 - j) * 100) << 12;
                int idx = (unsigned short)(short)(*(short*)(self + 0x8e) + 0x4000) >> 4;
                s16 cosv = data_02082214[idx * 2];
                s16 sinv = data_02082214[idx * 2 + 1];
                int offX = (int)(((s64)distFixed * cosv + 0x800) >> 12);
                int offZ = (int)(((s64)distFixed * sinv + 0x800) >> 12);
                int newX = *(int*)(self + 0x5c) + offX;
                int newZ = *(int*)(self + 0x64) + offZ;
                char* entry = self + (j << 6);
                pos.x = newX;
                pos.z = newZ;
                *(int*)(entry + 0x108) = newX;
                *(int*)(entry + 0x10c) = pos.y;
                *(int*)(entry + 0x110) = pos.z;
            }
            _ZN12CylinderClsn5ClearEv(cyl);
            _ZN12CylinderClsn6UpdateEv(cyl);
            cyl += 0x40;
        }
    }

    return 1;
}
