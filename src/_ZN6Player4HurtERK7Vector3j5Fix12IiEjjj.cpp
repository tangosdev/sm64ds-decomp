//cpp
#include "types.h"
// @symbol _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Player.h"
extern "C" {
extern int _ZN6Player8HasNoCapEv(char* c);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, void* v);
extern void func_ov002_020d5cec(char* c);
extern s16 Vec3_HorzAngle(const void* v0, const void* v1);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, int a, int b, int cc);
extern void _ZN6Player11ChangeStateERNS_5StateE(char* c, void* st);
extern int func_ov002_020d91e0(char* thiz, int damage, int doPre);

extern int data_ov002_02110094;
extern int data_ov002_0211010c;
}

extern "C" int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(struct Player *self, void* a, u32 b, int dmg, u8 arg4, u8 arg5, u8 arg6) {
    s16 ang;
    void* p360;
    int hasCap;

    if (func_ov002_020d82f0(((char*)self)) == 0) {
        return 0;
    }

    if (arg5 != 0) {
        if ((int)b < 2) {
            b += 2;
        }
    }

    hasCap = _ZN6Player8HasNoCapEv(((char*)self));
    if (hasCap != 0) goto L88;
    if (self->mCharacter == self->mParam) goto L88;
    if (self->mIsMetal != 0) goto L88;
    if (b == 0) goto L88;

    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(self->mCharacter, 0x22, ((char*)self) + 0x74);
    goto L9c;

L88:
    {
        int t = 0;
        if ((int)b > 1) t = 1;
        func_ov002_020d91b8(((char*)self), t);
    }

L9c:
    if (self->mIsMetal != 0) {
        b = 0;
    }
    p360 = *(void**)((char*)&self->mObjInMouth);
    if (p360 != 0) {
        int match = (*(u16*)((char*)p360 + 0xc) == 0xbf);
        if (match) {
            func_ov002_020d5cec((char*)p360);
            *(u16*)(((long long)(int)((char*)&self->mStateFlags))) &= ~2;
        }
        self->mUseAltBodyModel = 0;
        {
            int* p = (int*)(((long long)(int)((char*)*(void**)((char*)&self->mObjInMouth) + 0xb0)));
            *p |= 0x80000;
        }
        {
            int* p = (int*)(((long long)(int)((char*)*(void**)((char*)&self->mObjInMouth) + 0xb0)));
            *p &= ~0x40000;
        }
        {
            int* p = (int*)(((long long)(int)((char*)*(void**)((char*)&self->mObjInMouth) + 0xb0)));
            *p &= ~0x20000;
        }
        *(void**)((char*)&self->mObjInMouth) = 0;
    }

    self->mStateStep = 0;

    if (b != 0) {
        int neq = 0;
        if (self->mCharacter != self->mParam) neq = 1;
        if (neq) self->mStateStep = 2;
    }
    if ((int)b > 1) {
        self->mStateStep = 2;
    }
    if (self->mIsAirborne != 0) {
        self->mStateStep = 4;
    }

    ang = Vec3_HorzAngle(((char*)self) + 0x5c, a);
    if (AngleDiff(self->mAngleY, ang) > 0x4000) {
        *(u8*)(((long long)(int)((char*)&self->mStateStep))) += 1;
    }
    self->mPrevAngleY = ang + 0x8000;
    if (self->mStateStep & 1) {
        self->mAngleY = self->mPrevAngleY;
    } else {
        self->mAngleY = ang;
    }

    if (arg4 == 1) {
        u16 tv = data_ov002_020ff128[self->mParam];
        dmg = (dmg * tv) / 100;
    }
    self->mHorzSpeed = dmg;

    if (arg5 != 0) {
        u8 t = arg5 - 1;
        *(u8*)(((long long)(int)((char*)&self->mStateStep))) |= 0x10;
        self->mStateWork = t;
    }

    func_02014fa4((char*)&self->mAttackClsn);
    self->mHurtDamage = b;

    if (self->mIsUnderwater == 0) {
        if (self->mHurtDamage != 0) {
            if (arg6 != 0) {
                volatile struct { int x, y, z; } v;
                int y = self->mPosY;
                int z = self->mPosZ;
                int x = self->mPosX;
                y = y + 0x50000;
                v.x = x;
                v.y = y;
                v.z = z;
                if (*(volatile int*)((char*)&self->mHurtDamage) == 1) {
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xcb, x, y, z);
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xcc, v.x, v.y, v.z);
                } else {
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xc9, x, y, z);
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xca, v.x, v.y, v.z);
                }
            }
        }

        _ZN6Player11ChangeStateERNS_5StateE(((char*)self), &data_ov002_02110094);
        if (func_ov002_020d91e0(((char*)self), b << 8, 1) != 0) {
            *(u8*)(((long long)(int)((char*)&self->mStateStep))) &= 1;
            _ZN6Player11ChangeStateERNS_5StateE(((char*)self), &data_ov002_0211010c);
        }
    } else {
        _ZN6Player11ChangeStateERNS_5StateE(((char*)self), &data_ov002_021100ac);
        func_ov002_020d91e0(((char*)self), b << 8, 1);
    }

    return 1;
}
