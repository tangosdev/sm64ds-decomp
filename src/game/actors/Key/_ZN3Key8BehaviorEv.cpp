//cpp
// @symbol _ZN3Key8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Key.h"
typedef void (*VoidFn)();

extern "C" {
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void* m, int ang);
extern void MulMat4x3Mat4x3(void* d, void* a, void* b);
extern void SubVec3(void* d, void* a, void* b);
extern void Vec3_LslInPlace(void* v, int sh);
extern void AddVec3(void* d, void* a, void* b);
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int a, unsigned int b, int x, int y, int z, const void* v, void* cb);
extern void _ZN7fBase_c18MarkForDestructionEv(void* c);
extern int _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(void* c, void* w);
extern void _ZN5dCc_c5ClearEv(void* c);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void* c, void* cyl);
extern void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void* c, void* v);
extern void _ZN5dCc_c6UpdateEv(void* c);

extern char data_020a0e68;
extern int data_ov089_02132c40[];
extern int data_ov089_02132b40[];
extern int data_ov089_02132ca4[];
}

#define LAUNDER(p) (p)
struct C { virtual void dummy(); };
typedef void (C::*PMF)();
struct PmfEntry { PMF pmf; };
extern PmfEntry data_ov089_02132cec[];
#define PMFTABLE data_ov089_02132cec

int Key::Behavior()
{
    int vec[3];
    int p7[3];
    int pe[3];
    int v = unk_448;

    if (v != 0) {
        if (v == 3) {
            {
                char* o = *(char**)((char*)&unk_110);
                if (o != 0) {
                    int* s = (int*)(int)LAUNDER(o + 0x5c);
                    mPosX = s[0];
                    mPosY = s[1];
                    mPosZ = s[2];
                    {
                        char* o2 = *(char**)((char*)&unk_110);
                        int ang = *(short*)(o2 + 0x8e);
                        mAngleY = ang;
                    }
                }
            }
            if (mModelAnim.Finished() == 0) {
                Matrix4x3_FromTranslation(&data_020a0e68, mPosX, mPosY, mPosZ);
                Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
                MulMat4x3Mat4x3(mModelAnim.data.transforms, &data_020a0e68, &data_020a0e68);
                {
                    char* m = &data_020a0e68;
                    int t0 = *(int*)(m + 0x24);
                    int t1 = *(int*)(m + 0x28);
                    int t2 = *(int*)(m + 0x2c);
                    vec[2] = t2;
                    vec[0] = t0;
                    vec[1] = t1;
                }
                SubVec3(vec, ((char*)this) + 0x5c, vec);
                Vec3_LslInPlace(vec, 3);
                AddVec3(vec, ((char*)this) + 0x5c, vec);
                vec[1] = *(int*)((char*)mModelAnim.data.bones + 0xc) * 0x23 + vec[1];
                vec[1] = vec[1] - 0x48000;
                *(void**)((char*)&unk_464) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unk_464, 0x82, vec[0], vec[1], vec[2], 0, 0);
                *(void**)((char*)&unk_468) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unk_468, 0x83, vec[0], vec[1], vec[2], 0, 0);
            }
        }

        mModelAnim.Advance();
        func_ov089_02131f54(((char*)this));
        if (mModelAnim.Finished()) {
            int b = (actorID == 0x11a);
            if (b != 0) {
                if (mModelAnim.file != (BCA_File *)data_ov089_02132c40[1])
                    _ZN7fBase_c18MarkForDestructionEv(((char*)this));
            }
        }
        return 1;
    }

    if (_ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(((char*)this), ((char*)this) + 0x260)) {
        func_ov089_02131f54(((char*)this));
        _ZN5dCc_c5ClearEv((char*)&mdCcAcPos_c);
        return 1;
    }
    mEatingPlayer = 0;
    if (mSpinSpeed > 0x400) {
        short* q = (short*)(int)LAUNDER((char*)&mSpinSpeed);
        *q = *q - 0x100;
    } else if (mSpinSpeed == 0) {
        mSpinSpeed = 0x400;
    }
    {
        short* ang = (short*)(int)LAUNDER((char*)&mAngleY);
        *ang = *ang + mSpinSpeed;
    }
    _ZN8dActor_c9UpdatePosEP5dCc_c(((char*)this), 0);
    (((C*)((char*)this))->*PMFTABLE[mState].pmf)();
    func_ov089_02131f54(((char*)this));
    _ZN5dCc_c5ClearEv((char*)&mdCcAcPos_c);
    if (mState == 7) {
        p7[0] = data_ov089_02132b40[0];
        p7[1] = data_ov089_02132b40[1];
        p7[2] = data_ov089_02132b40[2];
        _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(((char*)this) + 0x220, p7);
    } else {
        pe[0] = data_ov089_02132ca4[0];
        pe[1] = data_ov089_02132ca4[1];
        pe[2] = data_ov089_02132ca4[2];
        _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(((char*)this) + 0x220, pe);
    }
    _ZN5dCc_c6UpdateEv((char*)&mdCcAcPos_c);
    return 1;
}
