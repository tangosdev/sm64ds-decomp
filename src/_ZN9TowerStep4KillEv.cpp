//cpp
// @symbol _ZN9TowerStep4KillEv
/* TowerStep::Kill() at ov015 0x0211233c, 0x64 bytes -- vtable slot 31.
 *
 * ATTRIBUTED BY THE VTABLE, AND THE OLD COMMENT WAS WRONG TWICE. _ZTV9TowerStep
 * (ov015 0x02114650) carries 0x0211233c at slot 31 --
 * config/arm9/overlays/ov015/relocs.txt has
 * `from:0x021146cc kind:load to:0x0211233c` -- while _ZTV10dBgActor_c's word at the
 * same slot (from:0x0210aeb4) relocates to 0x020ee55c, _ZN10dBgActor_c4KillEv. The
 * file used to say `recovered name: MovingBarSmall_Kill`; MovingBar is a
 * different class, with its own slot-31 override at 0x02111c3c reached through
 * _ZTV9MovingBar+0x7c. That comment is deleted rather than carried forward.
 *
 * The plainest member of the family: particle 0x48 at the step's own position,
 * no vertical offset, then the poof at that same position built from the fields
 * a SECOND time -- the ROM reloads mPos rather than reusing the words it just
 * passed -- then bank-3 sound 0x41 and MarkForDestruction. Only one Vector3
 * exists here, so there is no memberwise-copy question to answer.
 *
 * Particle::System::NewSimple stays spelled as its mangled name -- its
 * parameters are Fix12<int> BY VALUE and declaring the true types changes how
 * the caller passes them; argued in full in src/_ZN10dBgActor_c4KillEv.cpp. */
#include "TowerStep.h"
#include "Sound.h"

extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    u32 id, Fix12i x, Fix12i y, Fix12i z);

void TowerStep::Kill()
{
    Vector3 dustPos;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x48, mPosX, mPosY, mPosZ);
    dustPos.x = mPosX;
    dustPos.y = mPosY;
    dustPos.z = mPosZ;
    PoofDustAt(dustPos);
    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);
    MarkForDestruction();
}
