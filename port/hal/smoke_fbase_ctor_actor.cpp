// port/hal/smoke_fbase_ctor_actor.cpp -- fBase_c's non-polymorphic shadow
// constructor, for smoke_actor, smoke_savestate and smoke_persist.
//
// Lane SMOKELINK5, run link100 wave 10 round 5, applying out/SMOKELINK4B/
// remaining.md section B verbatim.
//
// ??0fBase_c@@QAE@XZ is referenced by src/_ZN8dActor_cC1Ev.cpp (on this batch)
// and by src/_ZN11dScMgBase_cC2Ev.cpp, both real C++ constructors, which is
// why they call the base constructor by its decorated name.
//
// port/hal/link21_rows.cpp:100-166 carries the whole ruling for why the answer
// is a shadow and not a seat -- the seat was tried first and measured to cost
// four unresolved names (??_7fBase_c@@6B@, ??_GfBase_c@@UAEPAXI@Z and two
// others) for the one it would close, and the shadow is also the faithful
// answer on the dispatch question: the DTORS-A test asks whether anything
// reads word 0 of the object between the constructor's vtable store and the
// derived class's own store, and the transcription this shadow calls
// (port/hal/actor_vtables.cpp:151) stores the ROM-shaped table and then makes
// direct calls only, no indirect call at all, so the shadow keeps the
// cartridge's table exactly where a seat would have replaced it with MSVC's
// own D1/D0-folded vtable.  None of that argument is about which target it is
// on, so it carries over verbatim.  Taking link21_rows.cpp whole here was
// measured to cost twenty: daStarGate_c's five state objects, eleven
// ov044/ov100 data symbols and three ROM functions, none of which this batch
// hosts.
//
// THE THREE LINES BELOW ARE COPIED CHARACTER FOR CHARACTER from
// hal/link21_rows.cpp lines 169, 171 and 172.  A mangle depends on the name,
// the class, the convention and the signature and on nothing else, so this
// non-polymorphic shadow named fBase_c produces exactly ??0fBase_c@@QAE@XZ
// while emitting no vftable, no RTTI and no deleting destructor -- and no
// target carries both this file and hal/link21_rows.cpp, so the two
// definitions cannot collide.

extern "C" void *_ZN7fBase_cC2Ev(char *self);   /* hal/actor_vtables.cpp:151 */

struct fBase_c { fBase_c(); };
fBase_c::fBase_c() { _ZN7fBase_cC2Ev((char *)this); }
