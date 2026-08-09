//cpp
// @symbol _ZN10BulletBill8BehaviorEv
/* recovered: named members + shared header, real C++ method
 *
 * One frame of the bullet: move, run the current state's tick out of the pmf
 * table, then resolve whatever the cylinder hit.
 *
 * unk_134 is the ID of whatever it collided with, and unk_130 the flags of that
 * hit -- three outcomes in priority order:
 *   0x10      hit by a mega character: the bullet dies, credits the kill, and
 *             spawns particle 0x8f at its own position
 *   0x3c0     any other qualifying contact: go to state 1 and play 0xb5
 *   otherwise the target is looked up, and ONLY a Player (actor ID 0xbf) that
 *             is not already in state +0x6fb gets hurt -- same state 1, same cue
 *
 * unk_100 is a per-state frame counter, incremented every frame and reset to 0
 * whenever the state changed during THIS frame -- which is why the old value is
 * kept in a local across the whole body rather than re-read.
 *
 * The cylinder is re-anchored 0x50000 BELOW the actor every frame before being
 * updated, matching the offset InitResources built it with.
 *
 * mPrevAngle* is published from mAngle* at the very end, so the state tick sees
 * last frame's values throughout.
 *
 * `#pragma opt_propagation off` around the func_02012694 call is original and
 * kept -- it is what keeps the position pointer and the constant from being
 * folded together.
 */
#include "BulletBill.h"
typedef int LocFix12;

struct Klass; typedef void (Klass::*PMF)();
struct M { PMF pmf; };


extern "C" {
void func_0200f760(void *self, void *cc);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *cc);
void *_ZN5Actor10FindWithIDEj(u32 id);
void _ZN6Player16IncMegaKillCountEv();
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32, int, int, int);
void _ZN9ActorBase18MarkForDestructionEv(void *self);
void func_02012694(int a, void *b);
void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, const Vector3 *pos);
void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, const Vector3 *pos, u32 a, LocFix12 b, u32 c, u32 d, u32 e);
void func_ov079_02126704(char *c);
void _ZN12CylinderClsn5ClearEv(void *self);
void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void *self, const Vector3 *pos);
void _ZN12CylinderClsn6UpdateEv(void *self);

extern M data_ov079_021282e0[];
}

int BulletBill::Behavior()
{
    char *c = (char *)this;
    int flags;
    u32 which;
    u32 id;

    func_0200f760(c, &mMovingCylinderClsnWithPos);
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);

    which = (u32)mState;
    {
        M *m = &data_ov079_021282e0[which];
        (((Klass *)c)->*(m->pmf))();
    }

    id = unk_134;
    if (id != 0) {
        flags = unk_130;
        if (flags & 0x10) {
            _ZN5Actor10FindWithIDEj(id);
            _ZN6Player16IncMegaKillCountEv();
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x8f, mPosX, mPosY, mPosZ);
            _ZN9ActorBase18MarkForDestructionEv(c);
            {
#pragma opt_propagation off
                void *p = &mCamSpacePosX;
                int n = 0x78;
                func_02012694(n, p);
            }
        } else if (flags & 0x3c0) {
            mState = 1;
            {
                Vector3 *pos = (Vector3 *)&mCamSpacePosX;
                _ZN5Sound9PlayBank0EjRK7Vector3(0xb5, pos);
            }
        } else {
            void *o = _ZN5Actor10FindWithIDEj(id);
            if (o != 0) {
                int eq = (*(u16 *)((char *)o + 0xc) == 0xbf);
                if (eq != 0) {
                    if (*(u8 *)((char *)o + 0x6fb) == 0) {
                        Vector3 pos;
                        pos.x = mPosX;
                        pos.y = mPosY;
                        pos.z = mPosZ;
                        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(o, &pos, 3, 0xc000, 1, 0, 1);
                        mState = 1;
                        {
                            Vector3 *p2 = (Vector3 *)&mCamSpacePosX;
                            _ZN5Sound9PlayBank0EjRK7Vector3(0xb5, p2);
                        }
                    }
                }
            }
        }
    }

    {
        u16 *h = (u16 *)&unk_100;
        *h = *h + 1;
        if (which != (u32)mState) {
            *h = 0;
        }
    }

    func_ov079_02126704(c);

    _ZN12CylinderClsn5ClearEv(&mMovingCylinderClsnWithPos);
    {
        Vector3 pos;
        pos.x = 0;
        pos.y = -0x50000;
        pos.z = 0;
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(&mMovingCylinderClsnWithPos, &pos);
    }
    _ZN12CylinderClsn6UpdateEv(&mMovingCylinderClsnWithPos);

    mPrevAngleX = mAngleX;
    mPrevAngleY = mAngleY;
    mPrevAngleZ = mAngleZ;
    return 1;
}
