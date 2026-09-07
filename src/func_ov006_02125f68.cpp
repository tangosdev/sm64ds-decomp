//cpp
// @symbol func_ov006_02125f68
/* recovered: dScMgSnowball_c collision ring, ov006 0x02125f68 (2528 bytes).
 * Once a tick the snowball fires 32 probes around itself, one every 0x800 of
 * angle at the current ball radius. Each probe records three flags -- solid,
 * push-out, in-water -- and the whole ring is re-fired (up to 0x21 times)
 * while anything is still hit, pushing the ball out along the averaged normal
 * each pass. If nothing is hit and the ball started inside water, the water
 * probes instead add buoyancy to the velocity. A breakable hit shrinks the
 * ball, spawns the two 0xf1/0xf2 particle systems scaled by the new radius,
 * and fires the break sound; a fast non-breakable hit fires the bump sound.
 * The tail re-walks the push flags to build the separation vector and
 * reflects the velocity off it.
 *
 * Four spellings are load-bearing, all measured against the ROM on 2004/b56:
 *   - `hit` is dScMgSnowball_c::Vec2, whose empty destructor keeps it an
 *     8-byte aggregate in the frame (sample at sp+0x38, hit at sp+0x40, acc at
 *     sp+0x48). As two ints or an int[2] it is scalarised into spill words at
 *     sp+0x18/0x1c and every later slot shifts.
 *   - `part` is Vector3 for the same reason: as three ints or an int[3] the
 *     two Particle::System::New calls keep the scaled coordinates in
 *     callee-saved registers instead of storing them at sp+0x90 and reloading
 *     for the second call (-6 instructions).
 *   - the four Particle rate fields re-read self->mBallSize instead of caching
 *     `mBallSize - 0x4000` in a local: the ROM re-loads 0xba0 for each of the
 *     six assignments.
 *   - `sample[1] += self->mBallSize` (not `0 + mBallSize`): the ROM adds
 *     through the register that just stored the zero, so the add survives.
 * The nine zero/constant scalars before the ring are the ROM's own frame
 * words sp+0x14..0x34 in source order; folding any pair together drops a
 * store the cartridge makes. The push loop divides its own `k * 0x10000`
 * rather than carrying an angle accumulator, which is what stops mwccarm
 * strength-reducing the /32 into a second induction variable. */
#include "dScMgSnowball_c.h"

extern "C" {
extern int func_ov006_021259d8(dScMgSnowball_c *o, int *p);
extern int func_ov006_02125cdc(dScMgSnowball_c *o, int *p);
extern int func_ov006_02125bbc(dScMgSnowball_c *o, int *p);
extern void func_0203d388(int *p, int angle);
extern int func_0203d434(int *p);
extern int Vec2_Len(int *p);
extern void func_0203d630(int *p, int m);
extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 uniqueID, u32 effectID, int x, int y, int z, const void *dir, void *callback);
extern int *_ZN8Particle6System12FromUniqueIDEj(u32 uniqueID);
extern void func_02012718(int a, int b);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern s16 data_02082214[];
}

#define MUL12(a, b) ((int)(((s64)(a) * (b) + 0x800) >> 12))

extern "C" void func_ov006_02125f68(dScMgSnowball_c *self)
{
    int i;
    int k;
    int farHit;
    int anyHit;
    int initInside;
    int iter;
    int zNoHit;
    int zDry;
    int zOrigin;
    int pushDirX;
    int pushDirY;
    int floatDirX;
    int floatDirY;
    int zNotAllWater;
    int angle;
    int j;
    int sample[2];
    dScMgSnowball_c::Vec2 hit;
    int acc[2];
    int rotA[2];
    int rotB[2];
    int push[2];
    int rotPush[2];
    int initPt[2];
    int cdcCopy[2];
    int bbcCopy[2];
    int waterCopy[2];
    Vector3 part;
    int allWater;
    int len;
    int *sys;
    u32 id1;
    u32 id2;
    int cosv;
    int sc;
    int vx;
    int vy;

    i = 0;
    do {
        self->mProbeHit[i] = 0;
        self->mProbePush[i] = 0;
        self->mProbeWater[i] = 0;
        i = i + 1;
    } while (i < 0x20);
    farHit = 0;
    self->mHitBreakable = 0;

    initPt[0] = self->mPosX;
    initPt[1] = self->mPosY;
    initInside = func_ov006_021259d8(self, initPt);

    iter = 0;
    zDry = 0;
    pushDirX = 0;
    pushDirY = 0x800;
    zNotAllWater = 0;
    floatDirX = 0;
    floatDirY = 0x1000;
    zOrigin = 0;
    zNoHit = 0;
    do {
        anyHit = zNoHit;
        angle = zNoHit;
        for (j = zNoHit; j < 0x20; j++) {
            self->mProbeHit[j] = 0;
            sample[0] = 0;
            sample[1] = 0;
            sample[1] += self->mBallSize;
            func_0203d388(sample, angle);
            sample[0] += self->mPosX;
            sample[1] += self->mPosY;
            cdcCopy[0] = sample[0];
            cdcCopy[1] = sample[1];
            if (func_ov006_02125cdc(self, cdcCopy) != 0) {
                self->mProbeHit[j] = 1;
                self->mProbePush[j] = 1;
                anyHit = 1;
                if (Vec2_Len(&self->mVelX) >= 0x800)
                    farHit = 1;
                hit.x = sample[0];
                hit.y = sample[1];
            } else {
                bbcCopy[0] = sample[0];
                bbcCopy[1] = sample[1];
                if (func_ov006_02125bbc(self, bbcCopy) != 0) {
                    self->mProbeHit[j] = 1;
                    anyHit = 1;
                    self->mProbePush[j] = 1;
                    self->mHitBreakable = 1;
                    hit.x = sample[0];
                    hit.y = sample[1];
                }
            }
            waterCopy[0] = sample[0];
            waterCopy[1] = sample[1];
            if (func_ov006_021259d8(self, waterCopy) != 0)
                self->mProbeWater[j] = 1;
            else
                self->mProbeWater[j] = zDry;
            angle = (s16)(angle + 0x800);
        }

        acc[0] = zOrigin;
        acc[1] = zOrigin;
        angle = zOrigin;
        if (anyHit != 0) {
            for (i = 0; i < 0x20; i++) {
                if (self->mProbeHit[i] == 1) {
                    rotA[0] = pushDirX;
                    rotA[1] = pushDirY;
                    func_0203d388(rotA, angle);
                    acc[0] += rotA[0];
                    acc[1] += rotA[1];
                }
                angle = (s16)(angle + 0x800);
            }
            func_0203d434(acc);
            if (Vec2_Len(&self->mVelX) < 0x800)
                func_0203d630(acc, Vec2_Len(&self->mVelX));
            else
                func_0203d630(acc, pushDirY);
            self->mPosX -= acc[0];
            self->mPosY -= acc[1];
        } else if (initInside == 1) {
            allWater = 1;
            for (i = 0; i < 0x20; i++) {
                if (self->mProbeWater[i] == 1) {
                    rotB[0] = floatDirX;
                    rotB[1] = floatDirY;
                    func_0203d388(rotB, angle);
                    acc[0] += rotB[0];
                    acc[1] += rotB[1];
                } else {
                    allWater = zNotAllWater;
                }
                angle = (s16)(angle + 0x800);
            }
            func_0203d434(acc);
            self->mVelX += acc[0];
            self->mVelY += acc[1];
            if (allWater == 1)
                self->mAllWater = 1;
        }
        iter = iter + 1;
    } while (anyHit == 1 && iter < 0x21);

    if (self->mHitBreakable == 1) {
        len = Vec2_Len(&self->mVelX);
        self->mBallSize -= len;
        if (self->mBallSize < 0x4000)
            self->mBallSize = 0x4000;
        if (len >= 0x800) {
            int pz = self->mBallSize;
            int py = (self->mScrollY - hit.y) + 0x110000;
            int px = hit.x - 0x80000;
            part.x = px;
            part.y = py;
            part.z = pz;
            id1 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                0, 0xf1, part.x * 8, part.y * 8, part.z * 8, 0, 0);
            id2 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                0, 0xf2, part.x * 8, part.y * 8, part.z * 8, 0, 0);
            sys = _ZN8Particle6System12FromUniqueIDEj(id1);
            if (sys != 0) {
                sys[0x44 / 4] = (s16)((self->mBallSize - 0x4000) * 11 / 60 + 0x3000);
                sys[0x48 / 4] = (s16)((self->mBallSize - 0x4000) * 36 / 10 / 60 + 0x1333);
                sys[0x4c / 4] = (s16)((self->mBallSize - 0x4000) * 26 / 10 / 60 + 0x1666);
                sys[0x50 / 4] = (s16)((self->mBallSize - 0x4000) * 8 / 10 / 60 + 0x666);
            }
            sys = _ZN8Particle6System12FromUniqueIDEj(id2);
            if (sys != 0) {
                sys[0x48 / 4] = (s16)((self->mBallSize - 0x4000) * 62 / 10 / 60 + 0x1ccc);
                sys[0x50 / 4] = (s16)((self->mBallSize - 0x4000) * 45 / 10 / 60 + 0x1800);
            }
            if (self->mState == 1) {
                func_02012718(0x169, hit.x);
                if (self->mSoundPending == 1) {
                    func_02012718(0x16a, self->unk_b9e8);
                    self->mSoundPending = 0;
                }
            }
        }
    } else if (farHit == 1 && self->mState == 1) {
        func_02012718(0x168, hit.x);
    }

    k = 0;
    self->mSoundPending = 0;
    push[0] = 0;
    push[1] = 0;
    do {
        if (self->mProbePush[k] == 1) {
            rotPush[0] = 0;
            rotPush[1] = 0x1000;
            func_0203d388(rotPush, (s16)(k * 0x10000 / 32));
            push[0] -= rotPush[0];
            push[1] -= rotPush[1];
        }
        k = k + 1;
    } while (k < 0x20);

    if (push[0] == 0 && push[1] == 0)
        return;

    func_0203d434(push);
    sc = MUL12(self->mVelX, push[0]) + MUL12(self->mVelY, push[1]);
    if (self->mHitBreakable == 1) {
        sc = MUL12(sc, 0x1800);
        self->mVelX -= MUL12(sc, push[0]);
        self->mVelY -= MUL12(sc, push[1]);
        cosv = data_02082214[((unsigned short)_ZN4cstd5atan2E5Fix12IiES1_(
            self->mVelY, self->mVelX) >> 4) * 2];
        vx = self->mVelX;
        self->mVelX = vx / 8 + MUL12(vx, cosv) / 4;
        vy = self->mVelY;
        self->mVelY = vy / 8 + MUL12(vy, cosv) / 4;
    } else {
        sc = MUL12(sc, 0x1200);
        self->mVelX -= MUL12(sc, push[0]);
        self->mVelY -= MUL12(sc, push[1]);
    }
}
