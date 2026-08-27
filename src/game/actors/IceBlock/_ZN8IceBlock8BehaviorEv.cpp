//cpp
// @symbol _ZN8IceBlock8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "IceBlock.h"

struct VObj {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30();
    virtual void OnHit(); /* slot 31 = 0x7c */
};

extern "C" {
extern void _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void* c, int a, int b);
extern int _ZN4cstd4fdivEii(int a, int b);
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern void _ZN7fBase_c18MarkForDestructionEv(void* c);
extern unsigned int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int a, unsigned int b, int x, int y, int z, void* v, void* cb);
extern void* _ZN8Particle6System12FromUniqueIDEj(unsigned int id);
extern void func_02012694(int a, void* p);
extern void _ZN5dCc_c5ClearEv(void* p);
extern void _ZN5dCc_c6UpdateEv(void* p);
}

int IceBlock::Behavior()
{
    Vector3 v;
    int yo;
    void* s;
    char* p;
    unsigned int id;

    _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(((char*)this), 0, 0);

    if (mMeltTimer != 0) {
        mScale = _ZN4cstd4fdivEii(mMeltTimer << 12, 0x1e000);

        if (DecIfAbove0_Byte((unsigned char*)((char*)&mMeltTimer)) == 0) {
            p = *(char**)((char*)&mContainedActor);
            if (p != 0) {
                if (*(unsigned short*)(p + 0xc) == 0xb2)
                    *(unsigned char*)(p + 0x49f) = 0;
            }
            _ZN7fBase_c18MarkForDestructionEv(((char*)this));
        } else {
            ((int*)&v)[0] = mPosX;
            ((int*)&v)[1] = mPosY;
            ((int*)&v)[2] = mPosZ;
            yo = (int)(((long long)mScale * 0x96000 + 0x800) >> 12);
            ((int*)&v)[1] = v.y + yo;

            mParticleHandle1 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                mParticleHandle1, 0x77, v.x, v.y, v.z, 0, 0);
            mParticleHandle2 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                mParticleHandle2, 0x78, v.x, v.y, v.z, 0, 0);

            id = mParticleHandle1;
            if (id != 0) {
                s = _ZN8Particle6System12FromUniqueIDEj(id);
                if (s != 0)
                    *(int*)((char*)s + 0x44) = mScale * 0xf;
            }
            id = mParticleHandle2;
            if (id != 0) {
                s = _ZN8Particle6System12FromUniqueIDEj(id);
                if (s != 0)
                    *(int*)((char*)s + 0x44) = mScale * 0xf;
            }
        }
        return 1;
    }

    if (mdCcAc_c.otherOwner != 0) {
        if ((mdCcAc_c.hitFlags & 0x40000) != 0) {
            mMeltTimer = 0x1e;
            func_02012694(0x7a, ((char*)this) + 0x74);
            func_ov081_02127be0(((char*)this));
        }
        if ((mdCcAc_c.hitFlags & 0x4000) != 0) {
            ((VObj*)((char*)this))->OnHit();
        }
    }

    _ZN5dCc_c5ClearEv((char*)&(*(u8 *)&mdCcAc_c));
    _ZN5dCc_c6UpdateEv((char*)&(*(u8 *)&mdCcAc_c));
    return 1;
}
