//cpp
// @symbol _ZN8IceSheet4KillEv
/* IceSheet::Kill() at ov018 0x02112880, 0x60 bytes -- vtable slot 31.
 *
 * ATTRIBUTED BY THE VTABLE. _ZTV8IceSheet (ov018 0x02113b34) carries 0x02112880
 * at slot 31 (0x02113bb0) and _ZTV10dBgActor_c carries _ZN10dBgActor_c4KillEv at the
 * same slot, so this is this class's own override of dBgActor_c's Kill. The RTTI
 * agrees on the class: _ZTI15daObjIceBoard_c (ov018 0x02113af0) is a
 * __si_class_type_info with ONE base at subobject offset 0, ov002 0x021089ec --
 * dBgActor_c, the tree's dBgActor_c.
 *
 * The sheet shatters: the sound first, then three particle systems -- 0x74,
 * 0x75, 0x76 -- all at the actor's own position, and then it is gone.
 *
 * THE POSITION IS RE-READ PER CALL. The ROM issues three `ldr` from
 * [this,#0x5c/#0x60/#0x64] ahead of each of the three calls, nine loads in all,
 * rather than loading the triple once into callee-saved registers. Hoisting
 * them into locals is one instruction shorter per call and does not reproduce;
 * naming the fields at each call site is what asks for the loads the cartridge
 * has. That is also the only reason this body needs no Vector3 at all -- unlike
 * its siblings it never spawns a poof, so nothing here is passed by reference
 * except the camera-space triple.
 *
 * Particle::System::NewSimple stays spelled as its mangled name: its parameters
 * are Fix12<int> BY VALUE, and declaring the true types changes how the caller
 * passes them and breaks the bytes. The argument is made in full in
 * src/_ZN10dBgActor_c4KillEv.cpp -- notes/mwccarm-codegen.md 6az.
 *
 * mCamSpacePos is read as a Vector3 through its first member, which is how the
 * rest of the tree spells that triple. */
#include "IceSheet.h"
#include "Sound.h"

extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    u32 id, Fix12i x, Fix12i y, Fix12i z);

void IceSheet::Kill()
{
    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x74, mPosX, mPosY, mPosZ);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x75, mPosX, mPosY, mPosZ);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x76, mPosX, mPosY, mPosZ);
    MarkForDestruction();
}
