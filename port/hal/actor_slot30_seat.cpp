/* THE SHARED SEAT FOR VTABLE SLOT 30, Actor::OnAimedAtWithEggReturnVec.
 *
 * WHAT SLOT 30 IS. It is the point a thrown Yoshi egg homes at. The egg's
 * in-flight state (state 1, main func_ov002_020ecf94) calls
 * func_ov002_020ecd18 EVERY FRAME, and that function -- guarded only by
 * `o != 0` -- does a genuine virtual `v30()` on the actor the egg is aimed at
 * (src/func_ov002_020ecd18.cpp:96). So this slot fires on every frame of every
 * egg's flight, not only on impact.
 *
 * WHAT THE ROM PUTS THERE, READ OUT OF THE ROM. Every Actor-width vtable the
 * port fills has 0x020100dc in slot 30 -- the arm9 base body,
 * _ZN5Actor25OnAimedAtWithEggReturnVecEv -- with exactly ONE exception in the
 * whole tree: Whomp (ov079 0x02123b54), which has its own override and is
 * seated separately in hal/actor_classes_wf_enemy.cpp. That was established by
 * reading the slot-30 relocation (vtable + 30*4) of all 178 resolvable host
 * vtables against config/<module>/relocs.txt, not by pattern-matching the
 * fills. The ovNNN_syms.txt notes in port/ record the same reading
 * independently ("slot 30 0x020100dc _ZN5Actor25OnAimedAtWithEggReturnVecEv").
 *
 * So there is nothing to decompile here and nothing to invent: the body
 * already exists, byte-matched on all three mwccarm 1.2 configs, in
 * src/_ZN5Actor25OnAimedAtWithEggReturnVecEv.cpp. It was simply not in any
 * slice, so it never reached the host link, and 85 of the 86 fills trapped the
 * slot instead. It now joins the link on slice_gate50.txt beside
 * Actor::OnAimedAtWithEgg and Actor::OnTurnIntoEgg, the two Actor tail bodies
 * already there for the same reason.
 *
 * WHY A THUNK AT ALL: THE SRET CONTRACT. This is the thing every one of those
 * 85 trap comments names as the blocker ("returns a Vector3 by value, an ABI a
 * thunk cannot bridge", "an SRET method no __fastcall thunk shape models").
 * It is bridgeable, and hal/actor_classes_wf_enemy.cpp has been doing it for
 * Whomp all along -- this file is that same shape, written once.
 *
 * MSVC x86 returns a 12-byte struct through a hidden pointer. For a THISCALL
 * member that means: `this` in ecx, the hidden result pointer PUSHED as the
 * one stack argument, the CALLEE pops it, and the callee returns the pointer
 * in eax. A `__fastcall` function of three parameters models exactly that --
 * first goes to ecx, second to edx, the third lands on the stack -- and
 * __fastcall is callee-clean, so the pop is right too.
 *
 * The caller side already emits that sequence: func_ov002_020ecd18 spells the
 * dispatch as a real C++ virtual returning a struct by value
 * (`struct Vec3 tmp = ((struct VObj *)o)->v30();`), which is why it byte-
 * matches the ROM and why nothing about the ROM side has to change.
 *
 * WHAT THE BODY DOES, so that "seated" means "correct" and not merely "does
 * not fault": it returns the actor's position (+0x5c..+0x64) with the result
 * of vtable slot 29, Actor::OnAimedAtWithEgg (the egg lock-on radius, base
 * 0x14000 == 20.0), added to y. That slot-29 dispatch is why the fills that
 * still trapped 29 had to be seated at the same time -- otherwise this body
 * would simply move the freeze one slot along.
 */
extern "C" {

struct PortActorVec3 { int x, y, z; };

/* src/_ZN5Actor25OnAimedAtWithEggReturnVecEv.cpp, slice_gate50.txt.
   The matched TU's own spelling: hidden result pointer first, then `this`. */
void _ZN5Actor25OnAimedAtWithEggReturnVecEv(PortActorVec3 *ret, void *self);

/* The seat every Actor-width fill installs in slot 30. Three parameters, the
   Whomp shape: self -> ecx, the unused second -> edx, the hidden result
   pointer -> the one stack slot the caller pushed and this function pops. */
void *__fastcall port_actor_s30_base(void *self, void *, void *out)
{
    _ZN5Actor25OnAimedAtWithEggReturnVecEv((PortActorVec3 *)out, self);
    return out;
}

}  /* extern "C" */
