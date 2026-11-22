//cpp
#include "types.h"
#include "dCamera_c.h"
#include "daPly_c.h"
#include "math/CalcLib.h"
#include "nitro/fx/fx_cp.h"

extern "C" {

}

/* Camera state main for following the player. The eye target is the
 * player's position raised by 0x64000. With no mLookTarget the camera looks
 * there along the player's facing, from 0x48000 away, or 0xd0000 while the
 * player's mOwlFarCamera is set. With one, it aims at the midpoint between the
 * two (pulled back to at most 0x30000 from the eye), and turns off the line
 * to the target by up to 0x2000, less as the target gets farther than 0x200000
 * from the player. The pitch is clamped to +-0x2800. */
int dCamera_c::func_02005110() {
    Vector3 eye;
    Vector3 aim;
    Vector3 *target;
    s32 horzAngle;
    s32 eyeDist;
    {
        Vector3 *p = &this->mTargetPlayer->mPos;
        eye.x = p->x;
        eye.y = p->y;
        eye.z = p->z;
        eye.y += 0x64000;
    }
    eyeDist = 0x48000;
    target = this->mLookTarget;
    if (target == 0) {
        aim.x = eye.x;
        aim.y = eye.y;
        aim.z = eye.z;
        if (((daPly_c *)this->mTargetPlayer)->mOwlFarCamera != 0) {
            eyeDist = 0xd0000;
        }
        horzAngle = this->mTargetPlayer->mAngle.y;
    } else {
        Vector3 diff;
        Vector3 sum;
        Vector3 newAim;
        s32 dist;
        s32 scale;
        s16 fall;
        Vec3_Add(&sum, target, &eye);
        aim.x = sum.x;
        aim.y = sum.y;
        aim.z = sum.z;
        Vec3_AsrInPlace(&aim, 1);
        dist = Vec3_Dist(&eye, &aim);
        if (dist > 0x30000) {
            Vec3_Sub(&diff, &aim, &eye);
            scale = FX_Div(0x30000, dist);
            Vec3_MulScalarInPlace(&diff, scale);
            Vec3_Add(&newAim, &eye, &diff);
            aim.x = newAim.x;
            aim.y = newAim.y;
            aim.z = newAim.z;
        }
        fall = 0x2000;
        {
            Vector3 *playerPos = &this->mTargetPlayer->mPos;
            s32 fallDist = Vec3_Dist(this->mLookTarget, playerPos);
            s32 t = fallDist - 0x200000;
            if (t > 0) {
                s16 scaled = (((((s64)t) << 2) + 0x800) >> 12);
                if (scaled > fall) {
                    scaled = fall;
                }
                fall -= scaled;
            }
        }
        s16 diffAngle = Vec3_HorzAngle(this->mLookTarget, &eye) - this->mAngleY;
        fall = (diffAngle >= 0) ? (-fall) : (fall);
        /* Does nothing, but without it the build does not match the ROM. */
        if ((!newAim.x) && (!newAim.x)) {
        }
        horzAngle = (s16)(Vec3_HorzAngle(this->mLookTarget, &eye) + fall);
    }
    {
        s32 vert = Vec3_VertAngle(&aim, &eye);
        if (vert < (-0x2800)) {
            vert = -0x2800;
        } else if (vert > 0x2800) {
            vert = 0x2800;
        }
        this->ApproachLookAtAndAngles(&aim, horzAngle, vert);
    }
    {
        u32 r = this->ScaleDistByFov(eyeDist);
        if (this->ApproachDist(r) == 0) {
            this->ClampPosAboveTarget();
        }
    }
    return 1;
}
