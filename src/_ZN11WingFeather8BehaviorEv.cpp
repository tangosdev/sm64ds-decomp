//cpp
// @symbol _ZN11WingFeather8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "WingFeather.h"
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int u32;
typedef long long s64;

extern "C" {
void* _ZN5Actor13ClosestPlayerEv(void* self);
int _ZN6Player15IsCollectingCapEv(void* self);
void _ZN5Actor13SmallPoofDustEv(void* self);
void _ZN9ActorBase18MarkForDestructionEv(void* self);
u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 kind, u32 sub, int x, int y, int z, void* vec, void* cb);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* clsn);
void func_020383fc(void* p);
int _ZNK12WithMeshClsn10IsOnGroundEv(void* self);
void _Z15ApproachLinear2Rsss(short* ref, short target, short rate);
void _Z14ApproachLinearRiii(int* ref, int target, int rate);
u8 DecIfAbove0_Byte(u8* p);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 kind, int x, int y, int z);
void* _ZN5Actor10FindWithIDEj(u32 id);
void _ZN6Player16InitWingFeathersEb(void* self, int b);
void _ZN12CylinderClsn5ClearEv(void* self);
void _ZN12CylinderClsn6UpdateEv(void* self);
void func_ov002_020b2c44(char* c);
}

extern u8 data_0209f2d8;
extern s16 data_02082214[];
extern void* data_0209f318;

#pragma opt_common_subs off

int WingFeather::Behavior()
{
    {
        int b = (data_0209f2d8 == 1);
        if (!b) {
            void* p = _ZN5Actor13ClosestPlayerEv(((char*)this));
            if (*(int*)((char*)p + 8) != 0 || _ZN6Player15IsCollectingCapEv(p) != 0) {
                _ZN5Actor13SmallPoofDustEv(((char*)this));
                _ZN9ActorBase18MarkForDestructionEv(((char*)this));
                return 1;
            }
        }
    }

    unk_380 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        unk_380, 0x4a, mPosX, mPosY, mPosZ, 0, 0);

    _ZN5Actor9UpdatePosEP12CylinderClsn(((char*)this), 0);
    func_020383fc((char*)&mWithMeshClsn);

    if (_ZNK12WithMeshClsn10IsOnGroundEv((char*)&mWithMeshClsn) != 0) {
        _Z15ApproachLinear2Rsss((short*)((char*)&unk_37c), 0, 0x50);
        _Z14ApproachLinearRiii((int*)((char*)&unk_098), 0, 0x555);
        if (DecIfAbove0_Byte((u8*)((char*)&unk_384)) == 0) {
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd2, mPosX, mPosY, mPosZ);
            _ZN9ActorBase18MarkForDestructionEv(((char*)this));
        }
    } else {
        short* angp = (short*)((((s64)((char*)&unk_37c))));
        short old_ang = *angp;
        *angp = old_ang + 0x400;
        u16 newv = *(u16*)((char*)((char*)&unk_300) + 0x7c);
        int idx = ((newv >> 4) << 1) + 1;
        unk_098 = (int)(((s64)unk_378 * data_02082214[idx] + 0x800) >> 12);
    }

    _Z14ApproachLinearRiii((int*)((char*)&unk_378), 0x10000, 0x332);

    {
        int idx2 = (unk_37c >> 4) << 1;
        unk_08c = data_02082214[idx2] + 0x4000;
        int idx3 = ((unk_37c >> 4) << 1) + 1;
        unk_090 = data_02082214[idx3] * 2 - 0x6000;
    }

    {
        int b = (data_0209f2d8 == 1);
        if (!b) {
            unk_08e = *(s16*)((char*)data_0209f318 + 0x17c) + 0x4000;
        }
    }

    u32 id = unk_148;
    if (id != 0) {
        void* a = _ZN5Actor10FindWithIDEj(id);
        if (a != 0) {
            int eq = (*(u16*)((char*)a + 0xc) == 0xbf);
            if (eq) {
                _ZN6Player16InitWingFeathersEb(a, 1);
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd2, mPosX, mPosY, mPosZ);
                _ZN9ActorBase18MarkForDestructionEv(((char*)this));
                return 1;
            }
        }
    }

    _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsn);
    _ZN12CylinderClsn6UpdateEv((char*)&mMovingCylinderClsn);
    func_ov002_020b2c44(((char*)this));
    return 1;
}
