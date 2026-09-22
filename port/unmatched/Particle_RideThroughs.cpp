// HOST COPIES of the three ARM ARGUMENT RIDE-THROUGHS in the particle
// subsystem -- src/func_0204a5bc.c, src/_ZN8Particle14SimpleCallback
// 14SpawnParticlesERNS_6SystemE.cpp and src/func_0203cbc0.c.
//
// THE SHAPE. On ARM the first four arguments live in r0-r3, so a function
// that does not name an argument does not disturb it: a callee can be reached
// with values its own prototype never mentions, and a bare `b other` veneer
// forwards every register it was handed. mwccarm's own output relies on this,
// and the decomp transcribes it faithfully -- which is correct, and which no
// stack-based host ABI can reproduce. The port keeps a catalog of these; this
// is the particle chapter of it.
//
// THE CHAIN THAT MATTERS, and what it cost. Particle::SimpleCallback::
// SpawnParticles is the spawn side of nine of the thirteen Callback vtables,
// so it runs the first time the Player kicks up dust:
//
//   SpawnParticles(this, &sys)            r0 = this, r1 = &sys
//     -> func_02049d60(engine)            ONE argument named; &sys still in r1
//        `void func_02049d60(int b, int a) { func_0204a5bc(a, b + 0x14); }`
//        reads that r1 as `a`, so it forwards (&sys, engine + 0x14)
//     -> func_0204a5bc()                  declared (void); it IS a veneer,
//        `ldr ip, [pc]; bx ip; .word 0x204c584`, so r0/r1 pass through
//     -> func_0204c584(&sys, engine + 0x14)   the particle emitter
//
// Hosted verbatim, every link drops its arguments and the emitter reads its
// system pointer off whatever the stack happened to hold: the first landing
// puff faulted in func_0204c584 with a definition pointer of 1.
//
// engine + 0x14 is the free PARTICLE list (func_0204a4c8 threads all 256
// nodes onto it), which is what the emitter pops from -- so the ride-through
// is carrying real, load-bearing values, not incidental register litter.
//
// The bodies below are the ROM's, unchanged; only the argument passing is
// made explicit. Nothing here decides anything the ROM did not.
#include "types.h"

extern "C" {

/* ---- 1. the veneer ------------------------------------------------------
   src/func_0204a5bc.c is `void func_0204a5bc(void) { func_0204c584(); }`,
   the transcription of a two-instruction trampoline. Its own caller already
   spells two arguments (`extern void func_0204a5bc(int a, int b)` in
   src/func_02049d60.c), so this just lets them through.
   PORT_HOST_ABI: ARM r0/r1 ride-through -- src trampoline is (void). */
void func_0204c584(void *self, void *list);
void func_0204a5bc(void *self, void *list) { func_0204c584(self, list); }

/* ---- 2. the callback that starts it -------------------------------------
   The ROM body is three statements:

       *(u32 *)((int)&sys + 0x1c) |= 2;      // mark the system spawning
       sys.field_3a = this->field_4;         // hand it the callback's value
       func_02049d60(data_0209ee74->p);      // and emit, with &sys in r1

   data_0209ee74 is the SysTracker and its +4 is the engine, so the third
   line means func_02049d60(engine, &sys). Written that way here.

   This is the C-named, cdecl definition the vtables in hal/particle_vtable.cpp
   point at directly -- replacing the src file removes the __thiscall method,
   so no forwarding face is needed for this one any more. */
void func_02049d60(void *engine, void *sys);
extern void *data_0209ee74;

/* THE TAG THIS BODY WAS OWED. Everything above is the reason; this is the
   reason written where port/tools/linkage.py can read it. Without it the tool
   lists this definition as a SHADOW -- a host body standing in front of a
   matched TU that could take over -- and that reading is wrong here, because
   the matched TU cannot be made to work under any stack ABI.

   The matched body's last statement is `func_02049d60(data_0209ee74->p)`.
   ONE argument. The System reference the method was ENTERED with is still in
   r1 at that call, and func_02049d60's own transcription reads a second
   parameter (`void func_02049d60(int b, int a)`), so on ARM it receives &sys
   without either side naming it. cdecl has no such register: nothing pushes
   &sys, and the callee takes whatever sits above the return address.

   That is measured, not deduced. Hosting the three bodies verbatim faulted in
   func_0204c584 with a definition pointer of 1 on the first landing puff, and
   slice_gate29.txt keeps this TU out of the gate-29 slice under the heading
   "NOT HERE: three ARM argument ride-throughs" for exactly that.
   PORT_HOST_ABI: ARM r1 ride-through; the matched TU never names &sys. */
void _ZN5dPa_c7level_c16simpleCallback_c14SpawnParticlesERN8Particle6SystemE(void *self,
                                                               void *sys)
{
    *(u32 *)((char *)sys + 0x1c) |= 2;
    *(s16 *)((char *)sys + 0x3a) = *(s16 *)((char *)self + 4);
    func_02049d60(*(void **)((char *)data_0209ee74 + 4), sys);
}

/* ---- 3. the teardown veneer ---------------------------------------------
   src/func_0203cbc0.c is the same trampoline shape aimed at operator delete
   (`ldr ip, [pc]; bx ip; .word 0x203cbf0`) and is transcribed `(void)` for
   the same reason. Particle::SysTracker::~SysTracker calls it as
   `func_0203cbc0(data_0209ee80)` to release the particle arena, so the
   pointer has to survive the hop. This one never crashed because nothing has
   torn a level down yet -- it would have freed whatever was in r0.

   THE DEFINITION LIVES IN unmatched/func_02073244_hostcopy.c: gate 31 hosted the
   same veneer for the Player's array-delete pair, identically, and the
   particles merge made the pair the fifteenth collision. The linker layer
   caught it, per the table. */

}  /* extern "C" */
