//cpp
// @symbol _ZN11daTBasket_c8BehaviorEv
/* Vtable slot 6. The cage bounces: a ground hit halves the rise speed and
 * kicks dust, while airborne frames trail secret sparkles (effect 0x119).
 * Grounded, once the secret chime is done (or its counter passes 0x3c), a
 * touching player jumps into the cage. */

#include "daTBasket_c.h"
#include "Player.h"

/* Sound::PlaySecretSound / Particle::System::New stay mangled: no shared
 * header declares them yet (d_a_wanwan / da1up precedent). The shadow helper
 * func_ov063_021169c4 is this class's own (only caller) but enrolled
 * separately; see daTBasket_c.h. */
extern "C" {
int _ZN5Sound15PlaySecretSoundEP8dActor_cPt(dActor_c *actor, u16 *timer);
unsigned int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int uniqueID, unsigned int effectID,
    int x, int y, int z, const void *dir, void *callback);
void func_ov063_021169c4(char *c);
}

int daTBasket_c::Behavior()
{
    int onGround = 0;
    int secretDone = 1;

    if (mMuteSecretSound == 0)
        secretDone = _ZN5Sound15PlaySecretSoundEP8dActor_cPt(this, (u16 *)&mSoundTimer);

    if (mWithMeshClsn.JustHitGround()) {
        int vertSpeed = mVertSpeed;
        mVertSpeed = (-vertSpeed) >> 1;
        LandingDust(false);
    } else if (mWithMeshClsn.IsOnGround()) {
        onGround = 1;
        /* Secret non-zero skips the timer gate; secret zero requires the chime
         * counter past 0x3c. The counter reads UNSIGNED: mSoundTimer is an s16
         * and the plain comparison sign-extends. */
        if (secretDone != 0 || (u16)mSoundTimer > 0x3c) {
            unsigned int id = mdCcAc_c.otherOwner;
            if (id != 0) {
                dActor_c *touched = dActor_c::FindWithID(id);
                if (touched != 0) {
                    if ((mdCcAc_c.hitFlags & 0x400000) != 0)
                        ((Player *)touched)->JumpIntoBooCage(*(Vector3 *)&mPosX);
                }
            }
        }
    }

    if (onGround == 0) {
        int z = mPosZ;
        int y = mPosY;
        int x = mPosX;
        unsigned int pid = (unsigned int)mParticleID;
        mParticleID = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            pid, 0x119, x, y + 0x64000, z, 0, 0);
    }

    UpdatePos(0);
    UpdateWMClsn(mWithMeshClsn, 0);
    func_ov063_021169c4((char *)this);
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}
