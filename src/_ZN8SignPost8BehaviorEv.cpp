//cpp
// @symbol _ZN8SignPost8BehaviorEv
/* SignPost::Behavior -- vtable slot 6. Real C++ method over the shared header.
 *
 * This was an extern "C" free function over a raw `char *c`, with every field
 * reached by literal offset and a local one-word Vector3. Naming the fields is
 * what proved four of them exist at all: 0x354 (mClsnResult), 0x380
 * (mShadowMat) and 0x584/0x588/0x58c (the two particle handles and the break
 * countdown) were all inside explicit `pad_` runs in include/SignPost.h until
 * this body was read. Byte-exact under 2004/b56 after the conversion.
 *
 * What it does, in order: drop the sign if the player carrying it is holding it
 * through a pause; widen the clip radius while it is being carried; run the
 * respawn timer for a sign that is hidden or fully pounded in, and put it back
 * at its home height once the player is far enough away; hand off to
 * dBgActor_c::UpdateKillByMegaChar; run the break countdown, trailing two
 * particles until it expires and poofing on the frame it does; otherwise tick
 * the pound cooldown, refresh collision, and drop the shadow.
 *
 * LD() is a no-op macro the legacy file used to MARK its read-modify-write
 * sites on mFlags. It is kept, with its name, so the marking survives -- it
 * emits nothing, and it is not the reason those sites take an address. */
#include "SignPost.h"
#include "Player.h"

enum Bool { FALSE, TRUE };
#define LD(p) ((int)(p))

extern "C" unsigned int data_0209b454;

extern "C" void _ZN6Player9DropActorEv(void *self);
extern "C" void func_ov002_020bb060(char *c);
extern "C" u8 DecIfAbove0_Byte(u8 *p);
extern "C" int _ZN8dActor_c13DistToCPlayerEv(void *self);
extern "C" void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *self);
extern "C" void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self);
extern "C" void func_ov002_020baf80(char *t);
extern "C" int _ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE(void *self, short a, short b, short c, int fix);
extern "C" int _ZN4dBgW9IsEnabledEv(void *self);
extern "C" void _ZN4dBgW7DisableEv(void *self);
extern "C" void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 a, u32 b, int c, int d, int e, const void *v, void *cb);
extern "C" void *_ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(u32 a, u32 b, int c, int d, int e, const void *v);
extern "C" void _ZN8dActor_c19DisappearPoofDustAtERK7Vector3(void *self, const struct Vector3 *vec);
extern "C" void func_ov002_020bae9c(char *c);
extern "C" void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(void *self, void *sm, void *m, int a, int b, int c, u32 j);
extern "C" void _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern "C" void func_ov002_020bbda4(char *c);
extern "C" void _ZN5dCc_c5ClearEv(void *self);
extern "C" void _ZN5dCc_c6UpdateEv(void *self);
extern "C" void func_ov002_020bafc0(char *self);

int SignPost::Behavior()
{
    struct Vector3 v;
    struct Vector3 vec, vec2;

    {
        enum Bool b = (enum Bool)((mFlags & 0x4000000) != 0);
        if (b != FALSE && (data_0209b454 & 0x4000000) && mHoldingPlayer != 0)
            _ZN6Player9DropActorEv(mHoldingPlayer);
    }

    {
        void *p = mHoldingPlayer;
        if (p != 0 && (enum Bool)((mFlags & 0x4000) != 0) != FALSE
            && *(int *)((char *)p + 0xc8) != 0) {
            u32 *fp;
            func_ov002_020bb060((char *)this);
            mClipRadius = 0x20000;
            fp = (u32 *)LD(&mFlags);
            *fp = *fp | 0x4000000;
        } else {
            u32 *fp;
            mClipRadius = 0x10000;
            fp = (u32 *)LD(&mFlags);
            *fp = *fp & ~0x4000000;
        }
    }

    if (mHidden != 0) {
        enum Bool b = (enum Bool)((mFlags & 8) != 0);
        if (b != FALSE && DecIfAbove0_Byte(&mRespawnDelay) == 0
            && _ZN8dActor_c13DistToCPlayerEv(this) > 0x7d0000)
            mHidden = 0;
        return 1;
    }

    if (mPoundsLeft == 0) {
        enum Bool b = (enum Bool)((mFlags & 8) != 0);
        if (b != FALSE && DecIfAbove0_Byte(&mRespawnDelay) == 0
            && _ZN8dActor_c13DistToCPlayerEv(this) > 0x7d0000) {
            mPoundsLeft = 2;
            mPosY = mHomePosY;
            _ZN10dBgActor_c21UpdateModelPosAndRotYEv(this);
            _ZN10dBgActor_c19UpdateClsnPosAndRotEv(this);
            func_ov002_020baf80((char *)this);
        }
    }

    if (_ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE(this, -0x2000, 0, 0, 0x46000))
        return 1;

    if (mBreakTimer != 0) {
        int x, y, z;
        if (_ZN4dBgW9IsEnabledEv(&mMeshCollider))
            _ZN4dBgW7DisableEv(&mMeshCollider);
        x = mPosX;
        z = mPosZ;
        y = mPosY + 0x50000;
        ((int *)&v)[0] = x;
        ((int *)&v)[1] = y;
        ((int *)&v)[2] = z;
        if (DecIfAbove0_Byte(&mBreakTimer) != 0) {
            *(void **)&mParticleHandle1 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                mParticleHandle1, 0x13a, v.x, v.y, v.z, 0, 0);
            *(void **)&mParticleHandle2 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
                mParticleHandle2, 0x13b, v.x, v.y, v.z, 0);
        } else {
            int x2, y2, z2;
            x2 = mPosX;
            z2 = mPosZ;
            y2 = mPosY + 0x28000;
            ((int *)&vec)[0] = x2;
            ((int *)&vec)[1] = y2;
            ((int *)&vec)[2] = z2;
            vec2 = vec;
            _ZN8dActor_c19DisappearPoofDustAtERK7Vector3(this, &vec2);
            func_ov002_020bae9c((char *)this);
            return 1;
        }
        _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
            this, &mShadowModel, &mShadowMat, 0x50000, 0x28000, 0x28000, 0xf);
        return 1;
    }

    DecIfAbove0_Byte(&mPoundCooldown);
    if (mHidden == 0)
        _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0);
    func_ov002_020bbda4((char *)this);
    _ZN5dCc_c5ClearEv(&mdCcAc_c);
    _ZN5dCc_c6UpdateEv(&mdCcAc_c);
    {
        int s = mClsnResult;
        if (s == 3) {
            func_ov002_020bafc0((char *)this);
        } else if (mPoundsLeft == 2 && (u32)s <= 1) {
            _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
                this, &mShadowModel, &mShadowMat, 0x50000, 0x28000, 0x28000, 0xf);
        }
    }
    return 1;
}
