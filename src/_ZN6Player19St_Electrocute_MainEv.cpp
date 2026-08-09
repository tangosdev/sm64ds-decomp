//cpp
// @symbol _ZN6Player19St_Electrocute_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Electrocution, per frame: a looping shock sound and three stacked particle
 * systems at the player's position raised 0x32000, then when the state timer
 * expires the two low bits of mStateWork decide where to go -- bit 0 selects
 * the "took damage" path over a plain exit, bit 1 selects the swimming exit
 * over the walking one.
 *
 * The volatile locals and the volatile reads of the particle handles are
 * load-bearing: they pin the order and the memory round-trips the original
 * compiled. They are kept verbatim.
 */
#include "Player.h"
extern "C" {
extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 a, u32 b, u32 c, Vector3* v, u32 d);
/* Particle::System::New takes Fix12<int> by value -- the mwccarm 6az wall,
   runbook section 7 -- so it stays extern "C" with scalars in those slots. */
extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 sysHandle, u32 kind, s32 scale, s32 p1, s32 p2, void* pos16, void* cb);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* self, void* state);
extern int func_ov002_020c5dec(void* c, int r1);
extern void Player_AdvanceAnims(void* self);

extern void* data_ov002_0211013c;   /* the walk state */
extern void* data_ov002_0211067c;   /* the swim state */
}

int Player::St_Electrocute_Main()
{
    Vector3* pos = (Vector3*)&mCamSpacePosX;
    volatile s32 x;
    volatile s32 y;
    volatile s32 zvar;
    s32 z;
    s32 yraw;

    mLoopingSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(mLoopingSoundHandle, 0, 0x10b, pos, 0);

    x = mPosX;
    yraw = mPosY;
    y = yraw;
    {
        s32 zraw = mPosZ;
        s32 yadjraw = yraw + 0x32000;
        z = zraw;
        zvar = zraw;
        y = yadjraw;
    }

    unk_628 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*(volatile u32*)&unk_628, 0xc1, x, y, z, 0, 0);
    mParticle2 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*(volatile u32*)&mParticle2, 0xc2, x, y, zvar, 0, 0);
    unk_630 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unk_630, 0xc3, x, y, zvar, 0, 0);

    if (mStateTimer == 0) {
        u8 flags = mStateWork;
        if ((flags & 1) == 0) {
            mInvincibleTimer = 0x24;
            flags = mStateWork;
            if ((flags & 2) == 0) {
                _ZN6Player11ChangeStateERNS_5StateE(this, &data_ov002_0211013c);
            } else {
                _ZN6Player11ChangeStateERNS_5StateE(this, &data_ov002_0211067c);
            }
        } else {
            if ((flags & 2) != 0) {
                func_ov002_020c5dec(this, 8);
            } else {
                mInvincibleTimer = 0x24;
                if (func_ov002_020c5dec(this, 4) == 0) {
                    _ZN6Player11ChangeStateERNS_5StateE(this, &data_ov002_0211013c);
                }
            }
        }
    }

    Player_AdvanceAnims(this);
    return 1;
}
