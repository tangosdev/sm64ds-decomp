//cpp
#include "Player.h"
#include "PathPtr.h"
#include "Particle__System.h"

/* Player on a CLPS surface of type 0xb (a current/conveyor tile): the tile's
 * CLPS word names a two-node path whose direction is the push vector. Spawns the
 * flow particle (func_ov002_020bf800), keeps the surface loop sound alive, and
 * unless the player is metal, uncontrolled or in a blocked state, pushes the
 * player along the path: grounded states get a position nudge scaled by
 * data_ov002_020ff210[def->unk_004], airborne states get their horizontal
 * velocity bent toward the path direction. Returns 1 when the push applied. */

extern "C" {
extern int _ZN6Player7IsStateERNS_5StateE(void *, void *);
extern unsigned int func_0203ad54(void *path);
extern s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern s16 Vec3_VertAngle(const Vector3 *v0, const Vector3 *v1);
extern s32 Vec3_HorzLen(const Vector3 *v);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 handle, u32 bank, u32 id, const void *pos, s16 e);
extern void func_ov002_020bf800(Player *self, Vector3_16f v);
extern int func_ov002_020e0478(void *self);

extern s16 data_02082214[];
extern s32 data_ov002_020ff210[];
extern int data_ov002_0211013c[];
extern int data_ov002_02110574[];
extern int data_ov002_02110184[];
extern int data_ov002_02110514[];
extern int data_ov002_021104e4[];
extern int data_ov002_02110364[];
extern int data_ov002_0211064c[];
extern int data_ov002_021101b4[];
extern int data_ov002_021102d4[];
}

#define FX_MUL(a, b) ((s32)(((s64)(a) * (s64)(b) + 0x800) >> 12))
#define SIN(a) data_02082214[((u16)(a) >> 4) * 2]
#define COS(a) data_02082214[((u16)(a) >> 4) * 2 + 1]

extern "C" int func_ov002_020bfa74(Player *self)
{
    s32 n;
    s32 i;
    u32 idx;
    s16 horz;
    s16 vert;
    s32 k;
    s32 scale;

    if (self->mSurfaceType != 0xb) return 0;
    if (self->mClpsWord1Lo != 0xff) {
        PathPtr path;
        Vector3_16f dir;
        Vector3 nodes[2];
        Vector3 vel;
        path.FromID(self->mClpsWord1Lo);
        n = path.NumNodes();
        for (i = 0; i < n; i++) {
            path.GetNode(nodes[i], i);
        }
        idx = (u8)func_0203ad54(&path);
        if (idx >= 4) idx = 1;
        horz = Vec3_HorzAngle(&nodes[0], &nodes[1]);
        vert = Vec3_VertAngle(&nodes[0], &nodes[1]);
        dir.y = SIN(vert);
        dir.x = FX_MUL(COS(vert), SIN(horz));
        dir.z = FX_MUL(COS(vert), COS(horz));
        func_ov002_020bf800(self, dir);
        self->mSurfaceSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(self->mSurfaceSoundHandle, 3, 0x90, &self->mCamSpacePosX, 0);
        if (self->mIsMetal != 0 || self->mIsNoControl != 0 || func_ov002_020e0478(self) != 0) return 0;
        if (self->mIsAirborne == 0) {
            if (_ZN6Player7IsStateERNS_5StateE(self, data_ov002_0211013c)
             || _ZN6Player7IsStateERNS_5StateE(self, &Player::ST_WAIT)
             || _ZN6Player7IsStateERNS_5StateE(self, data_ov002_02110574)
             || _ZN6Player7IsStateERNS_5StateE(self, data_ov002_02110184)
             || _ZN6Player7IsStateERNS_5StateE(self, data_ov002_02110514)
             || _ZN6Player7IsStateERNS_5StateE(self, data_ov002_021104e4)
             || _ZN6Player7IsStateERNS_5StateE(self, data_ov002_02110364)
             || _ZN6Player7IsStateERNS_5StateE(self, data_ov002_0211064c)) {
                if (self->mHorzSpeed <= 0) {
                    k = 0x8000;
                } else {
                    scale = data_ov002_020ff210[idx];
                    k = FX_MUL(scale, self->mHorzSpeed);
                }
                self->mPosX += FX_MUL(k, dir.x);
                self->mPosZ += FX_MUL(k, dir.z);
            }
        } else {
            if (_ZN6Player7IsStateERNS_5StateE(self, data_ov002_021101b4)
             || _ZN6Player7IsStateERNS_5StateE(self, data_ov002_021102d4)) return 1;
            vel.x = FX_MUL(self->mHorzSpeed, SIN(self->mPrevAngleY));
            vel.x += FX_MUL(dir.x, 0x2800);
            vel.z = FX_MUL(self->mHorzSpeed, COS(self->mPrevAngleY));
            vel.z += FX_MUL(dir.z, 0x2800);
            self->mPrevAngleY = _ZN4cstd5atan2E5Fix12IiES1_(vel.x, vel.z);
            self->mHorzSpeed = Vec3_HorzLen(&vel);
        }
        return 1;
    }
    return 0;
}
