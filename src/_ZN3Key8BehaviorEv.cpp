//cpp
// @symbol _ZN3Key8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Key.h"
typedef void (*VoidFn)();

extern "C" {
extern int _ZN9Animation8FinishedEv(void* a);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void* m, int ang);
extern void MulMat4x3Mat4x3(void* d, void* a, void* b);
extern void SubVec3(void* d, void* a, void* b);
extern void Vec3_LslInPlace(void* v, int sh);
extern void AddVec3(void* d, void* a, void* b);
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int a, unsigned int b, int x, int y, int z, const void* v, void* cb);
extern void _ZN9Animation7AdvanceEv(void* a);
extern void _ZN9ActorBase18MarkForDestructionEv(void* c);
extern int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(void* c, void* w);
extern void _ZN12CylinderClsn5ClearEv(void* c);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* c, void* cyl);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void* c, void* v);
extern void _ZN12CylinderClsn6UpdateEv(void* c);

extern char data_020a0e68;
extern int data_ov089_02132c40[];
extern int data_ov089_02132b40[];
extern int data_ov089_02132ca4[];
}

#define LAUNDER(p) ((long long)(int)(p))
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
                        unk_08e = ang;
                    }
                }
            }
            if (_ZN9Animation8FinishedEv((char*)&mAnimation) == 0) {
                Matrix4x3_FromTranslation(&data_020a0e68, mPosX, mPosY, mPosZ);
                Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, unk_08e);
                MulMat4x3Mat4x3(*(void**)((char*)&unk_128), &data_020a0e68, &data_020a0e68);
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
                vec[1] = *(int*)(*(char**)((char*)&unk_124) + 0xc) * 0x23 + vec[1];
                vec[1] = vec[1] - 0x48000;
                *(void**)((char*)&unk_464) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unk_464, 0x82, vec[0], vec[1], vec[2], 0, 0);
                *(void**)((char*)&unk_468) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unk_468, 0x83, vec[0], vec[1], vec[2], 0, 0);
            }
        }

        _ZN9Animation7AdvanceEv((char*)&mAnimation);
        func_ov089_02131f54(((char*)this));
        if (_ZN9Animation8FinishedEv((char*)&mAnimation)) {
            int b = (mActorID == 0x11a);
            if (b != 0) {
                if (unk_174 != data_ov089_02132c40[1])
                    _ZN9ActorBase18MarkForDestructionEv(((char*)this));
            }
        }
        return 1;
    }

    if (_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(((char*)this), ((char*)this) + 0x260)) {
        func_ov089_02131f54(((char*)this));
        _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsnWithPos);
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
        short* ang = (short*)(int)LAUNDER((char*)&unk_08e);
        *ang = *ang + mSpinSpeed;
    }
    _ZN5Actor9UpdatePosEP12CylinderClsn(((char*)this), 0);
    (((C*)((char*)this))->*PMFTABLE[mState].pmf)();
    func_ov089_02131f54(((char*)this));
    _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsnWithPos);
    if (mState == 7) {
        p7[0] = data_ov089_02132b40[0];
        p7[1] = data_ov089_02132b40[1];
        p7[2] = data_ov089_02132b40[2];
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(((char*)this) + 0x220, p7);
    } else {
        pe[0] = data_ov089_02132ca4[0];
        pe[1] = data_ov089_02132ca4[1];
        pe[2] = data_ov089_02132ca4[2];
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(((char*)this) + 0x220, pe);
    }
    _ZN12CylinderClsn6UpdateEv((char*)&mMovingCylinderClsnWithPos);
    return 1;
}
