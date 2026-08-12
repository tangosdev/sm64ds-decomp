//cpp
// @symbol _ZN4Exit8BehaviorEv
/* recovered: named members + shared header, real C++ method
 *
 * Two modes, and mHorzSpeed picks which.
 *
 * ZERO -- watching. Transform the player into door-local space and test the
 * box: |x| within mScaleX, y within [0, mScaleY], and the sign of z CHANGED
 * since last frame. That last test is why mScaleZ is stored every frame: the
 * door fires on the crossing, not on being inside, so walking along the plane
 * never triggers it. Height up the box picks the exit band mAngleZ (0/1/2).
 *
 * Then one of three outcomes, in order of priority:
 *   - Player::Unk_020c9e5c(7) -> wipe out with fader 6;
 *   - a tilted door (mAngleX != 0) -> wipe with fader 5, and hunt actor 0x12a
 *     within 0x12c000 to choose which music cue plays;
 *   - otherwise -> arm the second mode by storing the crossing depth in
 *     mHorzSpeed, point the camera at the exit, and cue 0x1e.
 *
 * NON-ZERO -- pulling. Each frame drags the player 0x20000 further along local
 * -z, re-projecting through the inverse matrix, and clamps at -0x300000, at
 * which point the fader is stopped and the scene handed over.
 *
 * The whole thing is gated on data_02092110 < 0, so the door is inert unless
 * the level is in the right state.
 */
#include "Exit.h"

typedef struct Mtx { int m[12]; } Mtx;

extern "C" {
extern void MulVec3Mat4x3(void *in, void *m, void *out);
extern void InvMat4x3(void *in, void *out);
extern void func_ov002_020b0a0c(void *c);
extern void _ZN5Scene20SetAndStopColorFaderEv(void);
extern int _ZN6Player20IsStateEnteringLevelEv(void *p);
extern int _ZN6Player12Unk_020c9e5cEh(void *p, int a);
extern void StartExitFaderWipe(int a);
extern void _ZN6Camera9SetFlag_3Ev(void *cam);
extern void _ZN6Player17SetNoControlStateEhih(void *p, int a, int b, int d);
extern char *_ZN5Actor15FindWithActorIDEjPS_(unsigned int id, void *prev);
extern int Vec3_Dist(void *a, void *b);
extern void func_02012790(int id);
extern void _ZN6Camera10LookAtExitER5Actor(void *cam, void *a);

extern unsigned char data_0209f250;
extern char *data_0209f394[];
extern signed char data_02092110;
extern char data_0209f5e8[];
extern char *data_0209f318;
extern Mtx data_020a0e68;
}

int Exit::Behavior()
{
    int out1[3];
    int out2[3];
    char *player = data_0209f394[data_0209f250];
    if (mHorzSpeed != 0) {
        MulVec3Mat4x3(player + 0x5c, &mInvMat, out1);
        if (out1[2] > mHorzSpeed || out1[2] < -0x300000) {
            out1[2] = mHorzSpeed;
            data_020a0e68 = *(Mtx *)&mInvMat;
            InvMat4x3(&data_020a0e68, &data_020a0e68);
            MulVec3Mat4x3(out1, &data_020a0e68, player + 0x5c);
        }
        mHorzSpeed -= 0x20000;
        if (mHorzSpeed < -0x300000) {
            mHorzSpeed = -0x300000;
            if (data_02092110 < 0) {
                func_ov002_020b0a0c(this);
                _ZN5Scene20SetAndStopColorFaderEv();
                *(short *)(data_0209f5e8 + 0xc) = 0x7fff;
            }
        }
    } else {
        if (data_02092110 < 0) {
            if (_ZN6Player20IsStateEnteringLevelEv(player) == 0) {
                MulVec3Mat4x3(player + 0x5c, &mInvMat, out2);
                {
                    int a = out2[0];
                    if (a < 0)
                        a = -a;
                    if (a <= mScaleX) {
                        if (out2[1] >= 0 && out2[1] <= mScaleY) {
                            if ((out2[2] ^ mScaleZ) < 0) {
                                char *cam;
                                mAngleZ = 0;
                                if (out2[1] > 0x100000) {
                                    mAngleZ = 2;
                                } else if (out2[1] > 0x60000) {
                                    mAngleZ = 1;
                                }
                                cam = data_0209f318;
                                if (_ZN6Player12Unk_020c9e5cEh(player, 7)) {
                                    func_ov002_020b0a0c(this);
                                    StartExitFaderWipe(6);
                                    _ZN6Camera9SetFlag_3Ev(cam);
                                } else {
                                    _ZN6Player17SetNoControlStateEhih(player, 6, -1, 0);
                                    if (mAngleX != 0) {
                                        char *o;
                                        func_ov002_020b0a0c(this);
                                        StartExitFaderWipe(5);
                                        _ZN6Camera9SetFlag_3Ev(cam);
                                        o = _ZN5Actor15FindWithActorIDEjPS_(0x12a, 0);
                                        while (o != 0) {
                                            if (Vec3_Dist(o + 0x5c, &mPosX) < 0x12c000)
                                                break;
                                            o = _ZN5Actor15FindWithActorIDEjPS_(0x12a, o);
                                        }
                                        if (o != 0) {
                                            func_02012790(0x16);
                                        } else if (data_02092110 == 0xd) {
                                            func_02012790(0x1e);
                                        } else {
                                            func_02012790(0x19);
                                        }
                                    } else {
                                        int t = (signed char)(int)(((unsigned int)mParam >> 0x18));
                                        if (t == 0x1b || t == 0x12) {
                                            func_ov002_020b0a0c(this);
                                            _ZN5Scene20SetAndStopColorFaderEv();
                                            *(short *)(data_0209f5e8 + 0xc) = 0x7fff;
                                        }
                                        mHorzSpeed = out2[2];
                                        _ZN6Camera10LookAtExitER5Actor(cam, this);
                                        func_02012790(0x1e);
                                    }
                                }
                            }
                        }
                    }
                }
                mScaleZ = out2[2];
            }
        }
    }
    return 1;
}
