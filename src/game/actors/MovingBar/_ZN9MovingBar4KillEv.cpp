//cpp
// @symbol _ZN9MovingBar4KillEv
/* MovingBar::Kill() at ov015 0x02111c3c, 0x7c bytes -- vtable slot 31.
 *
 * ATTRIBUTED BY THE VTABLE. _ZTV9MovingBar (ov015 0x0211458c) carries 0x02111c3c
 * at slot 31 -- config/arm9/overlays/ov015/relocs.txt has
 * `from:0x02114608 kind:load to:0x02111c3c` -- while _ZTV10dBgActor_c's word at the
 * same slot (from:0x0210aeb4) relocates to 0x020ee55c, _ZN10dBgActor_c4KillEv.
 * Different addresses in the same slot: this is the class's own override. The
 * file used to carry `recovered name: KnockDownPlank_Kill`, which names a
 * different class; the vtable is what settles it, so the comment is gone.
 *
 * Particle 0x10a, spawned 0xc8000 -- two hundred 20.12 units -- above the bar,
 * then the poof at the same point, bank-3 sound 0x41, then the bar destroys
 * itself. The offset is added to the built Vector3 rather than folded into the
 * load, which is the shape src/_ZN16daObjFallBlock_c4KillEv.cpp has and not the
 * one src/_ZN10dBgActor_c4KillEv.cpp has.
 *
 * The second Vector3 is copied MEMBERWISE on purpose: Vector3 declares a
 * destructor (types.h), so a whole-object assignment compiles to an ldm/stm
 * pair, four instructions where the ROM has six. Particle::System::NewSimple
 * stays spelled as its mangled name -- its parameters are Fix12<int> BY VALUE
 * and declaring the true types changes how the caller passes them. Both points
 * are argued in full in src/_ZN10dBgActor_c4KillEv.cpp. */
#include "MovingBar.h"
#include "Sound.h"

extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    u32 id, Fix12i x, Fix12i y, Fix12i z);

void MovingBar::Kill()
{
    Vector3 pos;
    Vector3 dustPos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0xc8000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x10a, pos.x, pos.y, pos.z);
    dustPos.x = pos.x;
    dustPos.y = pos.y;
    dustPos.z = pos.z;
    PoofDustAt(dustPos);
    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);
    MarkForDestruction();
}
