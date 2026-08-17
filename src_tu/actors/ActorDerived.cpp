//cpp
/* SHADOW translation unit -- arm9 / ActorDerived (Nintendo's dBase_c).
 *
 * NOT ENROLLED, AND NOT CANONICAL. This file contributes nothing to the ROM
 * build. The five one-function sources under src/ remain the enrolled owners of
 * 0x02013e80..0x02013f28. See notes/translation-unit-reconstruction-plan.md
 * (the design) and notes/tu-reconstruction-pilot-report.md (the worked example
 * this file follows -- there was no usable `tubuild.py create` candidate here,
 * because tu_map reports the arm9 `main` module as one 2984-function unit).
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S, for the three
 * ordinary definitions. mwccarm 2004/b56 emits one `.text` section per function
 * and lays those sections out in the REVERSE of source order, so the LAST
 * function of the ROM run is written FIRST. Reading order below is therefore
 * AfterInitResources, Spawn, func_02013edc.
 *
 * THE DESTRUCTOR IS NOT IN THAT ORDER, AND IS NOT WRITTEN HERE AT ALL.
 * ~ActorDerived is defined INLINE in include/ActorDerived.h (load-bearing --
 * Scene, Stage and every actor destructor below them inline its vptr store),
 * so its out-of-line copies are VAGUE-LINKAGE bodies. They are emitted because
 * this TU is the class's key-function TU: ActorDerived's first non-inline
 * virtual is AfterInitResources, defined here, which makes mwcc emit
 * _ZTV12ActorDerived, whose slots 16/17 reference D1 and D0 and pull both
 * out-of-line copies into existence. Nothing in the source text orders them.
 *
 * The ROM agrees, and the pair order is the proof. mwcc emits an OUT-OF-LINE
 * destructor definition as the fixed group D2, D0, D1 (pilot report sec 3), and
 * ActorBase -- whose destructor is defined out of line -- has exactly that in
 * the ROM: 0x02043d48 D2, 0x02043d78 D0, 0x02043dbc D1. An INLINE destructor's
 * vague-linkage copies come out as D1 then D0 with no D2 at all, which is what
 * the pilot measured for Platform, and it is exactly what this class has:
 * 0x02013e80 D1, 0x02013ea4 D0, no D2 anywhere in the image. The two classes
 * therefore corroborate each other's source form directly out of the cartridge.
 *
 * THIS TU ALSO EMITS, and this round neither declares nor verifies any of it:
 *   _ZTV12ActorDerived  .data  the ROM's is at 0x0208e4b8, supplied by the
 *                              module's gap object, not by this file
 *   _ZTI/_ZTS for ActorDerived and ActorBase
 * That is the ordinary consequence of compiling a key function, is inventoried
 * by `tubuild.py compile`, and is not licensed by the manifest entry.
 */
#include "ActorDerived.h"

/* Both veneer targets, spelled as their own src/ files spell them. Neither is
 * an ActorDerived member: they are ordinary arm9 functions 0x2f100 bytes away,
 * and the two 0xc-byte forwarders below are how this run reaches them. */
extern "C" {
extern void func_02042fe4(int a, int b, int c);
extern int  func_02042ffc(void *a, void *b);
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 4 -- ActorDerived::AfterInitResources(u32), 0x02013ef4, 0x34   */
/* ------------------------------------------------------------------------- */
/* recovered: real C++ method. vtable slot 2, the only functional override this
 * class makes -- every other slot still points at the ActorBase implementation.
 *
 * ALSO THE KEY FUNCTION. It is the first NON-INLINE virtual ActorDerived
 * declares (the destructor above it in the class is defined inline), so
 * defining it here is what makes this TU emit the vtable, and through the
 * vtable the two destructor variants. Moving this definition elsewhere would
 * move D1/D0 with it.
 */
void ActorDerived::AfterInitResources(u32 vfSuccess)
{
    if (vfSuccess == 1)
        MarkForDestruction();
    ActorBase::AfterInitResources(vfSuccess);
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 3 -- ActorDerived::Spawn, 0x02013ee8, size 0xc                 */
/* ------------------------------------------------------------------------- */
/* recovered: a 0xc-byte tail-call forwarder to func_02042ffc (0x02042ffc):
 *     ldr ip, [pc]; bx ip; .word 0x02042ffc
 * which is what mwccarm emits for a tail call under -interworking, `bx` being
 * the mode-preserving branch.
 *
 * THE PARAMETER LIST IS THE PROJECT'S RECONSTRUCTION, NOT A MEASUREMENT, and a
 * forwarder is the one shape that cannot testify about it: three words that
 * move no register observe nothing about their own arguments. What can be
 * observed is the CALLEE -- src/func_02042ffc.c is `int func_02042ffc(void *a,
 * void *b)`, TWO arguments, and it passes `(char *)b + 0x14`, i.e. an
 * ActorBase's sceneNode. So the callee's own recovered body agrees with this
 * name on the second parameter being an ActorBase* and says nothing at all
 * about the third and fourth. The name is left exactly as the config spells it;
 * see the return for what that costs.
 */
ActorBase *ActorDerived::Spawn(u32 actorID, ActorBase *parent, int a, int b)
{
    return (ActorBase *)func_02042ffc((void *)actorID, parent);
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_02013edc, 0x02013edc, size 0xc                       */
/* ------------------------------------------------------------------------- */
/* The same forwarder shape, to func_02042fe4 (0x02042fe4). Unnamed, and not an
 * ActorDerived member by any evidence -- but it sits INSIDE the class's run,
 * between D0 and Spawn, which is what puts it in this TU. */
extern "C" void func_02013edc(int a, int b, int c)
{
    func_02042fe4(a, b, c);
}
