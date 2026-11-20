//cpp
/**
 * Endless BGM / fog controller (registry profile MUGEN_BGM).
 *
 * param1 == 1 steers gMusicTrackModeOverride from the rotated offset; otherwise it
 * drives Fog depth/color from the rotated offset, star count and
 * gSaveBlock.file.mCharacter, then keeps Sound sub 0x2e playing while
 * func_ov100_02144f84 gates mHorzSpeed.
 *
 * This TU owns text only: ov002 delinks no .data here, so the _ZTV / _ZTI /
 * _ZTS group the class names is compiler-only output, compared against the
 * cartridge's own copies at ov002 0x0210b584 / 0x0210b544 / 0x0210b550.
 *
 * GetActiveFog is this TU's fog accessor: dScStage_c::RenderFog asks it
 * for the stored Fog instance, which it returns only while the stored area
 * is the current one.
 *
 * Notes:
 * - sEndlessStairsFogAreaId, data_ov002_02110af4 and sEndlessStairsFog are
 *   this TU's stored area, angle and Fog instance; gLocalPlayerId, gPlayers, 020a0ebc, 0209caa0,
 *   gCamera, 0208e434 and gCurrentAreaId are arm9 scene state this TU does
 *   not own.
 * - g_profile_MUGEN_BGM lives outside this TU.
 */

#include "daMugenBGM_c.h"
#include "types.h"
#include "Fog.h"
#include "decl_common.h"
#include "nitro/fx/fx.h"
#include "Sound.h"
#include "SaveBlock.h"
#include "dCamera_c.h"
#include "daPly_c.h"
#include "daDoor_c.h"
#include "math/CalcLib.h"

extern signed char sEndlessStairsFogAreaId;
extern signed char gCurrentAreaId;
extern Fog *sEndlessStairsFog;
extern u8 gLocalPlayerId;
extern daPly_c *gPlayers[];
extern Vector3 data_020a0ebc;
extern dCamera_c *gCamera;

u8 NumStars(void);
/* -------------------------------------------------------------------------- */
/* Factory: `return new` inherits fBase_c::operator new. */
/* -------------------------------------------------------------------------- */
daMugenBGM_c *daMugenBGM_c_classInit(void) {
    return new daMugenBGM_c;
}

/* -------------------------------------------------------------------------- */
int daMugenBGM_c::InitResources() {
    Fog *p;
    /* The param1 mask reads through a const view of `this`: `param1 &= 0xf`
       makes mwcc CSE the +8 field address into r1 (add r1,r4,#8 / ldr r0,[r1] /
       str r0,[r1]) and the function grows a word; the cartridge re-issues
       ldr/str [r4,#8]. Same lever as dScMgPachinko2_c::StartRound. */
    const daMugenBGM_c *ro = this;
    param1 = ro->param1 & 15;
    if (param1 != 1) {
        data_ov002_02110af4 = mAngle.y;
        p = new Fog;
        sEndlessStairsFog = p;
        if (p != 0) {
            p->Init(0, 0x700, 0xd00);
        }
        sEndlessStairsFogAreaId = mAreaId;
    }
    mAreaId = -1;
    mAngle.y = -mAngle.y;
    return 1;
}

/* -------------------------------------------------------------------------- */
int daMugenBGM_c::Behavior() {
    int vol;
    int flag;
    int depth;
    int color;
    Vector3 diff;
    Vector3 rotated;

    Vec3_Sub(&diff, &gPlayers[gLocalPlayerId]->mPos, &mPos);
    Vec3_RotateYAndTranslate(&rotated, &data_020a0ebc, mAngle.y, &diff);

    if (param1 == 1) {
        int z = rotated.z;
        int x = rotated.x;
        int az = (z < 0) ? -z : z;
        if (x < 0)
            x = -x;
        if (x < 0x320000) {
            int y = rotated.y;
            if (y > -0x12c000 && y < 0x5dc000 && az > 0x32000 && az < 0xc8000)
                gMusicTrackModeOverride = (z < 0) ? 2 : -1;
        }
        return 1;
    }

    depth = 0xffff;
    vol = 127;
    color = depth;
    flag = 0;

    if (NumStars() < 80 || gSaveBlock.file.mCharacter != 0) {
        int x = rotated.x;
        color = 0xd00;
        if (x < 0)
            x = -x;
        if (x < 0x200000) {
            int y = rotated.y;
            if (y > -0x32000 && y < 0x1000000 && rotated.z < 0) {
                int d;
                vol = 0;
                flag = 127;
                d = AngleDiff(mAngle.y, gCamera->mAngleY) - 0x2000;
                if (d < 0)
                    d = 0;
                else if (d > 0x4000)
                    d = 0x4000;
                depth = FX_MUL(rotated.z, (0x40 - (d >> 8))) + 0xffff;
                if (depth < 0x700)
                    depth = 0x700;
            }
        }
    }

    {
        Fog *fog = sEndlessStairsFog;
        if (fog != 0)
            fog->Init(0, (u16)depth, color);
    }

    if (mHorzSpeed == 0) {
        if (daDoor_c::func_ov100_02144f84() == 0)
            return 1;
        mHorzSpeed = 1;
    }

    if (Sound::PlaySub(0x2e, vol, flag, 0x1451, 0) != 0) {
        if (flag == 0)
            mHorzSpeed = 0;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
int daMugenBGM_c::Render() {
    return 1;
}

/* -------------------------------------------------------------------------- */
void daMugenBGM_c::OnPendingDestroy() {}

/* -------------------------------------------------------------------------- */
int daMugenBGM_c::CleanupResources() {
    return 1;
}

/* -------------------------------------------------------------------------- */
Fog *daMugenBGM_c::GetActiveFog() {
    if (sEndlessStairsFogAreaId == gCurrentAreaId)
        return sEndlessStairsFog;
    return 0;
}

/* -------------------------------------------------------------------------- */
/* Both destructors come from the inline `~daMugenBGM_c() {}` in
 * include/daMugenBGM_c.h; the `new daMugenBGM_c` in the factory instantiates
 * them. An out-of-line definition here emits D0 before D1 (the cartridge has
 * D1 at 0x020f1bc4, D0 at 0x020f1be8) plus a D2 the cartridge never had. */
