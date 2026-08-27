//cpp
// @symbol _ZN10dBgActor_c4KillEv
/* dBgActor_c::Kill() at ov002 0x020ee55c, 0x74 bytes -- vtable slot 31.
 *
 * THE SLOT THIS CLASS ADDS. dActor_c's table ends at slot 30; Kill is the one new
 * virtual dBgActor_c declares, and 97 of the 101 classes deriving from it have
 * exactly 32 slots because of this function. See include/dBgActor_c.h.
 *
 * Kill is also, necessarily, dBgActor_c's key function: the destructor is inline
 * on purpose (every subclass inlines its body rather than calling
 * _ZN10dBgActor_cD1Ev), so this is the first out-of-line virtual and this TU emits
 * _ZTV10dBgActor_c, _ZTI10dBgActor_c and the destructor variants. objisolate.py
 * reduces the object back to this one function before the byte gates see it.
 *
 * The particle spawns 0x64000 -- one 20.12 unit is 0x1000, so 100 units --
 * above the platform, and the poof and the sound both take the position by
 * reference. mCamSpacePos is read as a Vector3 through its first member, which
 * is how the rest of the tree spells that triple (see BulletBill::Behavior).
 *
 * Particle::System::NewSimple stays spelled as its mangled name: its parameters
 * are Fix12<int> BY VALUE, and declaring the true types changes how the caller
 * passes them and breaks the bytes. Same reason, same note, as
 * src/_ZN8dActor_c10PoofDustAtERK7Vector3.cpp -- notes/mwccarm-codegen.md 6az. */
#include "dBgActor_c.h"
#include "Sound.h"

extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    u32 id, Fix12i x, Fix12i y, Fix12i z);

void dBgActor_c::Kill()
{
    Vector3 pos;
    Vector3 dustPos;
    Fix12i x = mPosX;
    Fix12i y = mPosY + 0x64000;
    Fix12i z = mPosZ;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xa, pos.x, pos.y, pos.z);
    /* MEMBERWISE, NOT `dustPos = pos`. Vector3 declares a destructor (see
       types.h), so it is non-POD, and a whole-object assignment compiles to an
       ldm/stm pair -- four instructions where the ROM has six. Three field
       stores are what the cartridge does. */
    dustPos.x = pos.x;
    dustPos.y = pos.y;
    dustPos.z = pos.z;
    PoofDustAt(dustPos);
    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);
    MarkForDestruction();
}
