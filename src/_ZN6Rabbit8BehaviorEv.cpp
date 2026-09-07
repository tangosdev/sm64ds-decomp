//cpp
// @symbol _ZN6Rabbit8BehaviorEv
/* recovered: named members + shared header, real C++ method
 *
 * The glowing-rabbit chase, and it is mostly a conversation.
 *
 * The opening gate depends on the mode. Normally mIsDisabled latches the rabbit
 * out of the level until data_0209caa0[2] clears bit 0x20000; in mode 0x32 the
 * test inverts -- the rabbit runs only for the player whose character id
 * matches mCharacterId and latches itself off for everyone else, which is what
 * makes one actor behave differently per player.
 *
 * The talk sequence is mTalkState: 0 offers the message, 1 waits for the player's
 * talk state to end, 2 is done. Which message depends on mRabbitId, on whether
 * this rabbit has already been caught, and -- for the last one -- on whether
 * SaveData says all seven are found, which is the "you got them all" line.
 *
 * mState is the current animation descriptor. It is compared against four
 * ov085 objects rather than dereferenced as a type, so it stays an s32 and
 * those four comparisons are what identify the states.
 *
 * The block before Animation::Advance is the ARM/Itanium pointer-to-member
 * sequence written out -- adjustment word, virtual bit, vtable index or direct
 * address -- run on whatever mState points at. It is kept verbatim because
 * there is no recovered type for the descriptor to call a member through.
 *
 * TWO SPELLINGS HERE ARE LOAD-BEARING, both measured against the pre-image
 * rather than assumed:
 *
 *   `_ZN9Animation7AdvanceEv((char *)this + 0x350)` must offset from THIS.
 *   0x350 is mModelAnim's Animation base at +0x50, and writing it that way --
 *   `(char *)&mModelAnim + 0x50` -- costs a word. Offsetting from a typed
 *   sub-object's ADDRESS is not the same as offsetting from `this`; the same
 *   distinction cost Skeeter::Behavior a match once already.
 *
 *   The `(long long)(int)` round-trip on mEatenTimer stays. Removing it changes
 *   the size. Every other round-trip and every other offset in this function
 *   came out free -- the idiom is per-site, not per-function.
 *
 * NOT DONE HERE, but the evidence is now in the header: mModelAnim is almost
 * certainly a real ModelAnim. ModelAnim::SetAnim is called on +0x300, its
 * Animation base is used at +0x350 = +0x50, and 0x300 + sizeof(ModelAnim) is
 * 0x364 -- exactly where mState begins. Typing it would absorb mModelAnim.data.modelFile,
 * mModelAnim.data.materials and mModelAnim.speed, which Rabbit::Render and Rabbit::InitResources use, so
 * it wants its own slice.
 */
#include "Rabbit.h"

/* A LOCAL COORDINATE TRIPLE, NOT A Vector3 OBJECT. Vector3 declares a
   destructor (see include/types.h -- the ROM's __destroy_arr calls prove the
   type has one), so a Vector3 local would be destroyed at scope exit and this
   function would come out 8 bytes long. The ROM emits no cleanup for either of
   these, which is itself the evidence that they were never Vector3s: they are
   scratch x/y/z the code fills and reads back. */
typedef volatile struct { Fix12i x, y, z; } Vec3Scratch;


extern "C" s8 data_0209f2f8;
extern "C" s8 data_02092120;
extern "C" int data_0209caa0[];
extern "C" void* data_0209f33c;
extern "C" char data_ov085_021305c0;
extern "C" char data_ov085_0213068c;
extern "C" char data_ov085_021306ac;
extern "C" char data_ov085_021306bc;
extern "C" char data_ov085_021306dc;

extern "C" void* _ZN8dActor_c13ClosestPlayerEv(void* c);
extern "C" void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 a, u32 b, Fix12i c, Fix12i d, Fix12i e, void* f, void* g);
extern "C" void _ZN7Message11PrepareTalkEv(void);
extern "C" int func_02013890(int a, int b);
extern "C" void _ZN5Sound7PlaySubEjjj5Fix12IiEb(u32 a, u32 b, u32 c, Fix12i d, int e);
extern "C" int _ZN8SaveData22NumGlowingRabbitsFoundEv(void);
extern "C" int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void* thiz, void* ab, u32 id, const Vector3* pos, u32 e, u32 f);
extern "C" void func_02012694(int a, void* b);
extern "C" int _ZN6Player12GetTalkStateEv(void* c);
extern "C" void _ZN6Player9DropActorEv(void* c);
extern "C" int _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(void* c, void* w);
extern "C" void _ZN5dCc_c5ClearEv(void* c);
extern "C" void _ZN5dCc_c6UpdateEv(void* c);
extern "C" void func_ov085_0212bcc8(void* c);
extern "C" void func_ov085_0212bc78(void*, void*);
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* c, void* f, int i, Fix12i fx, u32 j);
extern "C" u16 DecIfAbove0_Short(void* p);
extern "C" void _ZN9Animation7AdvanceEv(void* c);
extern "C" void func_ov085_0212bdbc(void* c);
extern "C" void func_ov085_0212bedc(void* c);
extern "C" void _ZN8dActor_c9UpdatePosEP5dCc_c(void* c, void* cyl);
extern "C" void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void* c, void* w, u32 j);
extern "C" void func_ov085_0212a828(void* c);

int Rabbit::Behavior()
{
    char* c = (char*)this;
    void* r0p;

    if (data_0209f2f8 != 0x32) {
        if (mIsDisabled == 1) {
            if (data_0209caa0[2] & 0x20000)
                mIsDisabled = 0;
            return 1;
        }
    } else {
        r0p = _ZN8dActor_c13ClosestPlayerEv(c);
        if (r0p == 0)
            return 1;
        if (!(data_0209caa0[2] & 0x20000) || mCharacterId != *(u8*)((char*)r0p + 0x6d9)) {
            mIsDisabled = 1;
            return 1;
        }
        mIsDisabled = 0;
    }

    if (mRabbitId == 7 && !(data_0209caa0[1] & 0x40))
        data_0209f33c = c;

    if (mIsGlowing != 0) {
        Vec3Scratch pv;
        pv.x = mPosX;
        pv.y = mPosY;
        pv.z = mPosZ;
        pv.y = mPosY + 0x3c000;
        *(void**)&mGlowParticle = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(volatile u32*)&mGlowParticle, 0x10d, pv.x, pv.y, mPosZ, 0, 0);
    }

    {
        u32 temp_r1 = mTalkState;
        if (temp_r1 < 2) {
            int v = (mFlags & 0x40000) ? 1 : 0;
            if (v == 1) {
                void* temp_r4 = *(void**)&mTalkingPlayer;
                if (temp_r4 != 0) {
                    if (temp_r1 == 0) {
                        Vec3Scratch pos;
                        int var_r6;
                        int var_r2;
                        int t;
                        pos.x = mPosX;
                        pos.y = mPosY;
                        pos.z = mPosZ;
                        _ZN7Message11PrepareTalkEv();
                        t = mRabbitId;
                        if (t != 7) {
                            if (func_02013890(t, *(s32*)((char*)temp_r4 + 8)) == 0) {
                                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x27, 0x12, 0x7f, 0x15ccc, 0);
                                if (mRabbitId != 6) { var_r6 = 0x162; var_r2 = 0x11e; }
                                else { var_r6 = 0x160; var_r2 = 0x126; }
                            } else if (mIsGlowing == 0) {
                                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x26, 0x12, 0x7f, 0x15ccc, 0);
                                if (mRabbitId != 6) { var_r6 = 0x162; var_r2 = 0x122; }
                                else { var_r6 = 0x160; var_r2 = 0x12a; }
                            } else {
                                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x27, 0x12, 0x7f, 0x15ccc, 0);
                                if (_ZN8SaveData22NumGlowingRabbitsFoundEv() == 7) { var_r2 = 0x145; var_r6 = 0x160; }
                                else { var_r6 = 0x162; var_r2 = 0x144; }
                            }
                        } else {
                            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x27, 0x12, 0x7f, 0x15ccc, 0);
                            var_r2 = 0x13b; var_r6 = 0x160;
                        }
                        pos.y += 0x64000;
                        if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(temp_r4, c, var_r2, 0, 0, 0) == 1) {
                            func_02012694(var_r6, &mCamSpacePosX);
                            mTalkState = 1;
                        }
                    } else if (temp_r1 == 1 && _ZN6Player12GetTalkStateEv(temp_r4) == -1) {
                        _ZN6Player9DropActorEv(temp_r4);
                        mTalkState = 2;
                        *(u16*)((char*)temp_r4 + 0x6ce) |= 0x800;
                    }
                }
            }
        }
    }

    if (_ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(c, &mWithMeshClsn) != 0) {
        _ZN5dCc_c5ClearEv(&mdCcAc_c);
        if (mEatenByYoshi != 0) {
            if (unk_104 == 5)
                mHorzSpeed = 0;
            if (unk_104 == 0)
                _ZN5dCc_c6UpdateEv(&mdCcAc_c);
        }
        if (mEatenByYoshi == 1) {
            *(u8*)((long long)(int)(c + 0x42a)) = *(u8*)((long long)(int)(c + 0x42a)) + 1;
            if (mEatenTimer > 0x96) {
                mEatenByYoshi = 0;
                mEatenTimer = 0;
            }
        }
        func_ov085_0212bcc8(c);
        unk_426 = 2;
        func_ov085_0212bc78(c, &data_ov085_021306ac);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(void**)((char*)&data_ov085_021305c0 + 4), 0, 0x1000, 0);
        if (*(void**)&mTalkingPlayer == 0)
            *(void**)&mTalkingPlayer = _ZN8dActor_c13ClosestPlayerEv(c);
        return 1;
    }

    if (unk_426 != 0) {
        void* p = *(void**)&mState;
        if (p != (void*)&data_ov085_021306ac && p != (void*)&data_ov085_0213068c &&
            p != (void*)&data_ov085_021306bc && p != (void*)&data_ov085_021306dc) {
            func_ov085_0212bc78(c, &data_ov085_021306ac);
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(void**)((char*)&data_ov085_021305c0 + 4), 0, 0x1000, 0);
        }
        mHorzSpeed = 0;
    }

    DecIfAbove0_Short(&mStateTimer);

    {
        int* p = *(int**)&mState;
        if (p[2] != 0) {
            int* q = p + 2;
            int adj = q[1];
            void* thiz = c + (adj >> 1);
            void (*fn)(void*);
            if (adj & 1)
                fn = *(void(**)(void*))(*(char**)thiz + q[0]);
            else
                fn = (void(*)(void*))q[0];
            fn(thiz);
        }
    }

    _ZN9Animation7AdvanceEv(c + 0x350);

    {
        int v = (mFlags & 0x4000) ? 1 : 0;
        if (v != 0)
            func_ov085_0212bdbc(c);
        else
            func_ov085_0212bcc8(c);
    }

    if (data_0209f2f8 == 5 && data_02092120 == 3)
        func_ov085_0212bedc(c);

    {
        int v = (mFlags & 0x4000) ? 1 : 0;
        if (v == 0) {
            if (*(void**)&mState != (void*)&data_ov085_021306ac || unk_426 != 0)
                _ZN8dActor_c9UpdatePosEP5dCc_c(c, &mdCcAc_c);
            mAngleX = mPrevAngleX;
            mAngleY = mPrevAngleY;
            mAngleZ = mPrevAngleZ;
            _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(c, &mWithMeshClsn, 0);
            if (*(void**)&mState != (void*)&data_ov085_021306ac)
                func_ov085_0212a828(c);
        }
    }

    if (*(void**)&mState != (void*)&data_ov085_021306ac) {
        _ZN5dCc_c5ClearEv(&mdCcAc_c);
        _ZN5dCc_c6UpdateEv(&mdCcAc_c);
    }

    return 1;
}
