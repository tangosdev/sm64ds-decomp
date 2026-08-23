/* FOUR HOST COPIES of dScMgD3DBase_c framework slots -- 2, 10, 27 and 28 --
 * being func_ov006_020e70c0, func_ov006_020e7040, func_ov006_020e6d98 and
 * func_ov006_020e6d8c.  Run mg11, lane BNT.
 *
 * THIS FILE IS DELIBERATELY NOT NAMED AFTER A LANE, for the reason
 * unmatched/MgD3DBase_DeclConflict.cpp gives: dScMgD3DBase_c
 * (data_ov006_0213c62c, which the config misnames _ZTV17MgBounceAndPounce) is
 * the intermediate base of FOUR actor ids -- 0x174, 0x175, 0x180 and 0x181 --
 * and run mg11 seats all four in one wave.  Every seat needs these four
 * repairs.  ONE COPY SURVIVES the merge; the mg9 merge's MgShared4f38_* files
 * are the precedent.  Nothing here defines an Itanium name, so a second copy
 * arriving from a sibling branch is a duplicate to delete rather than an
 * LNK2005 to adjudicate -- but it should be deleted, not kept twice.
 *
 * ---- WHY THESE FOUR AND NOT THE OTHER THIRTEEN -------------------------
 *
 * All seventeen of dScMgD3DBase_c's override bodies were read out of
 * extracted/overlays/overlay_0006.bin at base 0x020bfec0 and checked against
 * their src TU for the three failure modes hal/method_faces.cpp's checklist
 * names -- wrong target, wrong arity, dropped receiver.  THIRTEEN ARE CLEAN.
 * Four are not, and all four are the SAME defect in two shapes: an argument
 * the ROM rides through in a register, which C has no way to spell.
 *
 *   slot  2  func_ov006_020e70c0, AfterInitResources
 *            ROM: push {r4,lr} / mov r4,r0 / bl 0x020b08f0 -- r1 is NEVER
 *            written, so the framework's FLAGS argument rides through into
 *            dScMgBase_c::AfterInitResources, which reads it
 *            (src/func_ov004_020b08f0.cpp takes `unsigned int flags`).
 *            src/_ZN17MgBounceAndPounce18AfterInitResourcesEj.cpp spells the
 *            call with ONE argument because that is the only way to spell an
 *            unnamed value in C.  This is dScMgSingle3DBase_c's slot 2 exactly
 *            -- port/unmatched/MgFlower_Slot2.cpp, run mg5 lane FLW -- at a
 *            different offset (+0x47e4 rather than +0x471c).
 *            IT ALSO RETURNS Particle::SysTracker::Initialise's result, and
 *            the framework READS that return: an actor whose
 *            AfterInitResources answers 0 goes on the cleanup list instead of
 *            the behaviour list and never ticks.  That is not a guess -- it is
 *            what this lane's first scene-373 run did, with the actor sitting
 *            on the cleanup list at `alive 2` on frame 1 while scene 388's sat
 *            on the behaviour list at `alive 1` (SM64DS_TRACE_LISTS=1).
 *
 *   slot 10  func_ov006_020e7040, BeforeRender
 *            ROM: stmdb sp!,{lr} / sub sp,#4 / bl 0x020b04f4 with r0 NEVER
 *            touched, so `this` rides through into
 *            dScMgBase_c::BeforeRender, which dereferences it on its first
 *            statement.  src/_ZN17MgBounceAndPounce12BeforeRenderEv.cpp
 *            declares and calls `func_ov004_020b04f4()` with NO argument --
 *            the DROPPED RECEIVER, failure mode 2.
 *            A DECLARATION CHANGE WOULD NOT HAVE FIXED THIS ONE.  The ROM body
 *            has a real frame (stmdb/sub sp), so MSVC gives the host body a
 *            real prologue too and the callee reads the CALLEE's stack, not
 *            the caller's.  Only a copy that names the receiver works.
 *
 *   slot 27  func_ov006_020e6d98  ->  func_ov004_020af27c
 *   slot 28  func_ov006_020e6d8c  ->  func_ov004_020af04c
 *            ROM: `ldr ip,[pc] / bx ip` and nothing else -- pure ARM tail
 *            veneers, so r0 rides through untouched.  Both src TUs spell the
 *            target `void f(void)`, and both ov004 bodies dereference their
 *            receiver on the first statement (func_ov004_020af27c reads
 *            self->unk_4630; func_ov004_020af04c dispatches through the
 *            object's own vtable).  hal/scene_mg.cpp's own base-table thunks
 *            mb_v27 and mb_v28 already pass `s` to those two bodies, so the
 *            framework half of the tree has always spelled it with the
 *            receiver and only this overlay's veneers had not.
 *            THE COPIES CALL ov004 DIRECTLY rather than through the veneer,
 *            and that loses nothing: the veneer's whole content is the tail
 *            jump.  Relying on MSVC keeping it a jmp -- so an argument the
 *            forwarder never names lands where its target reads it -- is the
 *            shape port/tools/tailjump_guard.py exists to protect, and these
 *            two are not on its roster.  Depending on an unguarded instance of
 *            a guarded hazard is the thing the guard exists to stop.
 *
 * NOT AN ABI WORKAROUND AND NOT TAGGED AS ONE.  Each body below is its src
 * TU's statements in order with the missing argument named.  The ROM rulings
 * for slots 27 and 28 are in port/tools/inferred_stub_adjudicated.txt (lane
 * BNT); slots 2 and 10 carry no marker and are not ruled on there.
 */

#include <cstdio>

extern "C" {

/* dScMgBase_c's own four, exactly as their src TUs declare them */
void func_ov004_020b08f0(void *c, unsigned int flags);
int  func_ov004_020b04f4(void *c);
void func_ov004_020af27c(void *c);
void func_ov004_020af04c(void *c);

/* the two Particle entry points the two repaired bodies keep */
int  _ZN8Particle10SysTracker10InitialiseEv(void *p);
int  _ZN8Particle9RenderAllEv(void);

}  /* extern "C" */

/* slot 2.  src/_ZN17MgBounceAndPounce18AfterInitResourcesEj.cpp verbatim with
   the flags argument named and forwarded.  The SysTracker lives at +0x47e4 on
   this base, which is what the ROM's one-word pool at 0x020e70e0 holds. */
extern "C" int port_mg_d3dbase_after_init(void *c, unsigned int flags)
{
    func_ov004_020b08f0(c, flags);
    return _ZN8Particle10SysTracker10InitialiseEv((char *)c + 0x47e4);
}

/* slot 10.  src/_ZN17MgBounceAndPounce12BeforeRenderEv.cpp verbatim with the
   receiver named and forwarded.  Both early exits are src's. */
extern "C" int port_mg_d3dbase_before_render(void *c)
{
    if (func_ov004_020b04f4(c) == 0)
        return 0;
    _ZN8Particle9RenderAllEv();
    return 1;
}

/* slots 27 and 28.  The two veneers, with the receiver the ROM rides through
   in r0 spelled as the argument it is. */
extern "C" void port_mg_d3dbase_slot27(void *c) { func_ov004_020af27c(c); }
extern "C" void port_mg_d3dbase_slot28(void *c) { func_ov004_020af04c(c); }
