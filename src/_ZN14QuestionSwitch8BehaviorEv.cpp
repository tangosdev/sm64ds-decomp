//cpp
// @symbol _ZN19daObjHatenaSwitch_c8BehaviorEv
/* daObjHatenaSwitch_c::Behavior -- vtable slot 6. Real C++ method over the shared
 * header. Callees whose ROM symbols carry by-value class parameters keep their
 * literal mangled extern "C" spellings; QSVec3 is a local plain-int triple, a
 * stack temp only. */
#include "QuestionSwitch.h"
#include "Message.h"
#include "Player.h"
#include "Sound.h"

struct QSVec3 { int x, y, z; };

extern "C" {
unsigned short DecIfAbove0_Short(unsigned short* p);
unsigned char DecIfAbove0_Byte(unsigned char* p);
void _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, int d, int e);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
}
extern u8 data_0209d684;
extern u8 data_0209d660;
extern u32 data_0209caa0[];

int daObjHatenaSwitch_c::Behavior()
{
    if (mSoundDelay != 0) {
        if (DecIfAbove0_Short(&mSoundDelay) == 0) {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, 0x7f, 0, 0x8777, 0);
        }
    }

    if (mTalking != 0) {
        Player *player = mTalkingPlayer;
        if (player != 0) {
            switch (player->GetTalkState()) {
            case 0:
                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, 0x14, 0x7f, 0x6b000, 0);
                mSoundDelay = 0x4b;
                if (mModelAnim.Finished() != 0) {
                    Message::PrepareTalk();
                    player->ShowMessage(*this, 0x183, 0, 1, 2);
                }
                break;
            case 1:
                break;
            default: {
                u8 st = data_0209d684;
                if (st == 1) {
                    Message::DisplaySaving(0x295);
                } else if (st == 2) {
                    if (player->HasFinishedTalking() != 0) {
                        Message::EndTalk();
                        mTalkingPlayer = 0;
                        mTalking = 0;
                    }
                } else {
                    if (data_0209d660 == 0) {
                        if (player->HasFinishedTalking() != 0) {
                            Message::EndTalk();
                            mTalkingPlayer = 0;
                            mTalking = 0;
                        }
                    }
                }
                break;
            }
            }
        } else {
            Player *p = ClosestPlayer();
            if (p->mIsAirborne == 0) {
                if (p->StartTalk(*this, true) != 0) {
                    mTalkingPlayer = p;
                }
            }
        }
    }

    {
        u8 f = mPressedThisFrame;
        if (f != 0) {
            if (DecIfAbove0_Byte((u8 *)&mPressTimer) == 0) {
                if (mActiveMeshCollider->IsEnabled() != 0) {
                    mActiveMeshCollider->Disable();
                }
                mActiveMeshCollider = &mMovingMeshCollider;
                data_0209caa0[1] |= 0x80000000;
                {
                    volatile QSVec3 v;
                    int y = mPosY + 0x64000;
                    int z = mPosZ;
                    int x = mPosX;
                    v.x = x;
                    v.y = y;
                    v.z = z;
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x6d, x, y, z);
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x6e, v.x, v.y, v.z);
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x6f, v.x, v.y, v.z);
                }
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x70, mPosX, mPosY, mPosZ);
                mTalking = 1;
            }
        } else if (f == 0) {
            mPressTimer = 8;
        }
    }

    if ((data_0209caa0[1] & 0x80000000) == 0) {
        mActiveMeshCollider = &mStaticMeshCollider;
    }

    if (UpdateClsnState() == 0) {
        if (mActiveMeshCollider == &mStaticMeshCollider) {
            mModelAnim.currFrame = 0;
        } else {
            if (mModelAnim.Finished() == 0) {
                mModelAnim.Advance();
                if (mModelAnim.Finished() != 0) {
                    Sound::PlayBank3(0, *(Vector3 *)&mCamSpacePosX);
                }
            }
        }
        UpdateModelTransform();
        UpdateClsnTransform();
    }

    mPressedThisFrame = 0;
    return 1;
}
