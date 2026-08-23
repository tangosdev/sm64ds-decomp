//cpp
#include "types.h"
// @symbol _ZN11WingFeather8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "WingFeather.h"
extern "C" {
void* _ZN8dActor_c13ClosestPlayerEv(void* self);
int _ZN6Player15IsCollectingCapEv(void* self);
void _ZN8dActor_c13SmallPoofDustEv(void* self);
void _ZN7fBase_c18MarkForDestructionEv(void* self);
u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 kind, u32 sub, int x, int y, int z, void* vec, void* cb);
void _ZN8dActor_c9UpdatePosEP5dCc_c(void* self, void* clsn);
void dBgCh_Actr_UpdateContinuous_Veneer(void* p);
int _ZNK10dBgCh_Actr10IsOnGroundEv(void* self);
void _Z15ApproachLinear2Rsss(short* ref, short target, short rate);
void _Z14ApproachLinearRiii(int* ref, int target, int rate);
u8 DecIfAbove0_Byte(u8* p);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 kind, int x, int y, int z);
void* _ZN8dActor_c10FindWithIDEj(u32 id);
void _ZN6Player16InitWingFeathersEb(void* self, int b);
void _ZN5dCc_c5ClearEv(void* self);
void _ZN5dCc_c6UpdateEv(void* self);
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
            void* p = _ZN8dActor_c13ClosestPlayerEv(((char*)this));
            if (*(int*)((char*)p + 8) != 0 || _ZN6Player15IsCollectingCapEv(p) != 0) {
                _ZN8dActor_c13SmallPoofDustEv(((char*)this));
                _ZN7fBase_c18MarkForDestructionEv(((char*)this));
                return 1;
            }
        }
    }

    unk_380 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        unk_380, 0x4a, mPosX, mPosY, mPosZ, 0, 0);

    _ZN8dActor_c9UpdatePosEP5dCc_c(((char*)this), 0);
    dBgCh_Actr_UpdateContinuous_Veneer((char*)&mWithMeshClsn);

    if (_ZNK10dBgCh_Actr10IsOnGroundEv((char*)&mWithMeshClsn) != 0) {
        _Z15ApproachLinear2Rsss((short*)((char*)&unk_37c), 0, 0x50);
        _Z14ApproachLinearRiii((int*)((char*)&mHorzSpeed), 0, 0x555);
        if (DecIfAbove0_Byte((u8*)((char*)&unk_384)) == 0) {
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd2, mPosX, mPosY, mPosZ);
            _ZN7fBase_c18MarkForDestructionEv(((char*)this));
        }
    } else {
        short* angp = (short*)((((s64)((char*)&unk_37c))));
        short old_ang = *angp;
        *angp = old_ang + 0x400;
        /* 0x300 is +0x1a8 inside the dBgCh_Actr at 0x158, which the cartridge's own
           ~WingFeather names (tools/dtor_members.py). It is unnamed there -- inside
           dBgCh_Actr's pad_135 -- so the offset is spelled out. */
        u16 newv = *(u16*)((char*)((char*)&mWithMeshClsn + 0x1a8) + 0x7c);
        int idx = ((newv >> 4) << 1) + 1;
        mHorzSpeed = (int)(((s64)unk_378 * data_02082214[idx] + 0x800) >> 12);
    }

    _Z14ApproachLinearRiii((int*)((char*)&unk_378), 0x10000, 0x332);

    {
        int idx2 = (unk_37c >> 4) << 1;
        mAngleX = data_02082214[idx2] + 0x4000;
        int idx3 = ((unk_37c >> 4) << 1) + 1;
        mAngleZ = data_02082214[idx3] * 2 - 0x6000;
    }

    {
        int b = (data_0209f2d8 == 1);
        if (!b) {
            mAngleY = *(s16*)((char*)data_0209f318 + 0x17c) + 0x4000;
        }
    }

    /* +0x24 inside the dCcAc_c at 0x124: dCc_c::otherOwner. */
    u32 id = mdCcAc_c.otherOwner;
    if (id != 0) {
        void* a = _ZN8dActor_c10FindWithIDEj(id);
        if (a != 0) {
            int eq = (*(u16*)((char*)a + 0xc) == 0xbf);
            if (eq) {
                _ZN6Player16InitWingFeathersEb(a, 1);
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd2, mPosX, mPosY, mPosZ);
                _ZN7fBase_c18MarkForDestructionEv(((char*)this));
                return 1;
            }
        }
    }

    _ZN5dCc_c5ClearEv((char*)&mdCcAc_c);
    _ZN5dCc_c6UpdateEv((char*)&mdCcAc_c);
    func_ov002_020b2c44(((char*)this));
    return 1;
}
