//cpp
// @symbol func_ov009_0211145c
/* Castle grounds birds -- ov009/daSBird_c, state 2 of the PMF table.
 *
 * ROM 0x0211145c..0x021115d8 (0x17c), the one function the daSBird_c class
 * run keeps as a free routine: the state-2 descriptor data_ov009_021138fc
 * in d_a_s_bird.cpp stores its address as a plain function pointer, not a
 * PMF, so it keeps the ROM symbol and is called with the bird as its first
 * argument. It sits between d_a_s_bird_head.cpp (ends 0x0211145c) and
 * d_a_s_bird.cpp (starts 0x021115d8), so it stays a one-function file;
 * folding it into either neighbour would merge the two halves.
 *
 * Leader flock setup: when the closest player is within 0x7d0 of the
 * leader, spawn (param1 & 0xf) - 1 follower birds attached to this one,
 * aim the flock at the fixed target and play sound 0x6a at the camera-
 * space position. Every bird then picks a random pitch (5000 - rand%4000)
 * and yaw, takes off at 0x28000 and enters state 3 with flag 0x10000
 * cleared.
 *
 * Byte-exact under 2004/b56. The function sat five words short for
 * roughly 2,000 earlier cells: written as `if (p2 == 0) return;` followed
 * by the stores and the sound call, b56 hoists the call's argument setup
 * above the last store and the pool constant cannot take r0. The lever is
 * the control-flow shape, not a spelling: with the stores in the taken arm
 * of `if (p2 != 0) { ... } else { return; }` and the call after the join,
 * the argument setup stays below the stores (the else-return is folded
 * into the conditional epilogue, so the bytes are otherwise identical).
 * The same shape appears at the sibling func_ov009_021115d8's call to
 * func_0201267c and at dScStage_c::Behavior's StopLoadedMusic call. The
 * body also compiles byte-exact as a daSBird_c member; the free form is
 * kept because the ROM symbol and the descriptor relocation are free.
 *
 * deslop leftovers:
 * - Vec3_Sub / Vec3_HorzLen: local externs, the spellings d_a_s_bird.cpp
 *   uses (no shared header). RandomIntInternal and func_0201267c are
 *   declared as their definitions are.
 * - Vec3_Sub is declared with void *, so &mPosX passes to it uncast.
 * - RandomIntInternal is cast unsigned at both uses: a signed % 4000 and
 *   a signed >> 16 change codegen.
 * - The ClosestPlayer result is a char* because the player position at
 *   +0x5c is read by raw offset, the way the siblings read it.
 */
#include "daSBird_c.h"
#include "common.h"

extern "C" {
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern void Vec3_Sub(void *out, void *a, void *b);
extern s32 Vec3_HorzLen(void *v);
extern void func_0201267c(unsigned int id, const Vector3 *v);
}

extern "C" void func_ov009_0211145c(daSBird_c *self)
{
    if (self->mIsLeader) {
        char *p2 = (char *)self->ClosestPlayer();
        if (p2 != 0) {
            struct Vector3 copy;
            struct Vector3 diff;
            Vec3_Sub(&diff, &self->mPosX, p2 + 0x5c);
            copy.x = diff.x;
            copy.y = diff.y;
            copy.z = diff.z;
            if (Vec3_HorzLen(&copy) > 0x7d0000)
                return;
            int n = (int)self->param1 & 0xf;
            if (n > 1) {
                for (int i = 0; i < n - 1; i++) {
                    void *a = dActor_c::Spawn(0x157, 0x10, self->mPos, 0, self->mAreaId, -1);
                    if (a)
                        ((daSBird_c *)a)->func_ov009_02111224(self->uniqueID);
                }
            }
            self->mTargetPos.x = -0x14000;
            self->mTargetPos.z = 0xff06a000;
        } else {
            return;
        }
        func_0201267c(0x6a, &self->mCamSpacePos);
    }
    self->mPrevAngleX = 5000 - (unsigned int)RandomIntInternal(&data_0209e650) % 4000;
    self->mPrevAngleY = (unsigned int)RandomIntInternal(&data_0209e650) >> 16;
    self->mFlySpeed = 0x28000;
    self->mState = 3;
    self->mFlags &= ~0x10000;
}
