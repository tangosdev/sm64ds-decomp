//cpp
/* Translation unit, reconstructed and promoted -- arm9 / dBase_c (Nintendo's dBase_c).
 *
 * CANONICAL. The ROM build compiles this file and takes all 5 functions from
 * this one object; the one-function sources that owned 0x02013e80..0x02013f28
 * are gone. See notes/translation-unit-reconstruction-plan.md (the design) and
 * notes/tu-reconstruction-pilot-report.md (the worked example this file
 * follows -- there was no usable `tubuild.py create` candidate here, because
 * tu_map reports the arm9 `main` module as one 2984-function unit).
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one `.text` section per function and lays those sections out in the
 * REVERSE of source order, so the LAST function of the ROM run is written FIRST.
 * Reading order below is therefore AfterInitResources, Spawn, func_02013edc, D0,
 * D1 -- and the emitted order comes out ROM-ascending, as `tubuild.py verify`
 * reports.
 *
 * NOTHING HERE IS A REAL dBase_c:: VIRTUAL, AND THAT IS THE WHOLE DESIGN OF
 * THIS FILE. The class declares exactly two virtuals of its own: the destructor,
 * defined INLINE in include/dBase_c.h, and AfterInitResources. So
 * AfterInitResources is the KEY FUNCTION, and defining it as a method makes this
 * TU emit `_ZTV7dBase_c` -- which the module's gap object already supplies
 * from ROM data. Both members are therefore spelled as extern "C" free functions
 * under their exact mangled names, and the two destructor variants with them.
 * The result is an object with no .data at all: 5 LICENSED, 0 COLLIDES-GAP,
 * 0 HOMELESS, and a whole-range link that reproduces the cartridge.
 *
 * Measured both ways, because the method form is the prettier one and it had to
 * be ruled out rather than assumed:
 *   real `void dBase_c::AfterInitResources(u32)`  -> 5/5 MATCH, but the
 *        link aborts, "Multiply-defined: virtual table for dBase_c ...
 *        Previously defined in _dsd_gap@main_44.o"
 *   extern "C", as below                               -> 5/5 MATCH and
 *        LINK-VERIFIED, 168 bytes IDENTICAL, module fidelity 106/106
 * The class DECLARATIONS in the header are untouched and still required -- they
 * are what give the class its slots.
 *
 * ~dBase_c IS STILL DEFINED INLINE IN THE HEADER and that is untouched too:
 * it is load-bearing for dScene_c, Stage and every actor destructor below them,
 * which inline its vptr store. Only the out-of-line copies the vtable points at
 * are spelled out here. See the destructor block at the bottom.
 *
 * AND THE ROM STILL PROVES THE INLINE FORM, independently of how this file is
 * written. mwcc emits an OUT-OF-LINE destructor definition as the fixed group
 * D2, D0, D1 (pilot report sec 3), and fBase_c -- whose destructor is defined
 * out of line -- has exactly that in the ROM: 0x02043d48 D2, 0x02043d78 D0,
 * 0x02043dbc D1. An INLINE destructor's vague-linkage copies come out D1 then D0
 * with no D2 at all, which is what the pilot measured for dBgActor_c and what this
 * class has: 0x02013e80 D1, 0x02013ea4 D0, no D2 anywhere in the image. The two
 * classes corroborate each other's source form straight off the cartridge.
 */
#include "dBase_c.h"

/* Both veneer targets, spelled as their own src/ files spell them. Neither is
 * an dBase_c member: they are ordinary arm9 functions 0x2f100 bytes away,
 * and the two 0xc-byte forwarders below are how this run reaches them. */
extern "C" {
extern void func_02042fe4(int a, int b, int c);
extern int  func_02042ffc(void *a, void *b);

/* The base's base-object destructor, which both variants below chain to, and
   this class's vtable as DATA -- named, not defined. The vtable reference has to
   be addend 0: the ROM literals at 0x02013ea0 (D1) and 0x02013ed4 (D0) are
   exactly 0x0208e4b8, which is config's `_ZTV7dBase_c`, the SLOT ARRAY. */
extern void *_ZN7fBase_cD2Ev(void *self);
extern void *_ZTV7dBase_c[];
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 4 -- dBase_c::AfterInitResources(u32), 0x02013ef4, 0x34   */
/* ------------------------------------------------------------------------- */
/* vtable slot 2, the only functional override this class makes -- every other
 * slot still points at the fBase_c implementation. If initialisation reported
 * VS_FAIL (1) the actor is marked for destruction, and either way the fBase_c
 * implementation runs afterwards.
 *
 * THIS IS THE KEY FUNCTION, AND THAT IS WHY IT IS NOT A METHOD HERE.
 * AfterInitResources is the first NON-INLINE virtual dBase_c declares (the
 * destructor above it in the class body is defined inline), so CW 1.2 emits
 * `_ZTV7dBase_c` into whichever TU defines it -- and the ROM's copy of
 * that vtable is already supplied as data by the module's gap object. Written as
 * a real `void dBase_c::AfterInitResources(u32)` this TU byte-verifies
 * 5/5, but the whole-range link then aborts:
 *
 *     mwldarm.exe: Multiply-defined: "virtual table for dBase_c"
 *     mwldarm.exe: Previously defined in _dsd_gap@main_44.o
 *
 * which is the documented key-function link wall. Spelled as an extern "C" free
 * function, no TU defines the key function, no vtable is emitted, and the link
 * goes through. The DECLARATION in the class is still required -- removing it
 * would delete slot 2 and shift the sixteen below it. Same arrangement
 * include/fBase_c.h uses for InitResources, and the same one that let
 * arm9/dActor_c and arm9/fBase_c reach link-verified.
 *
 * The qualified `self->fBase_c::AfterInitResources(...)` is a direct `bl`, not
 * a virtual dispatch -- which is what the ROM does, and what the method form's
 * unqualified `fBase_c::AfterInitResources(...)` also compiled to.
 */
extern "C" void _ZN7dBase_c18AfterInitResourcesEj(dBase_c *self,
                                                        u32 vfSuccess)
{
    if (vfSuccess == 1)
        self->MarkForDestruction();
    self->fBase_c::AfterInitResources(vfSuccess);
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 3 -- dBase_c::Spawn, 0x02013ee8, size 0xc                 */
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
 * fBase_c's sceneNode. So the callee's own recovered body agrees with this
 * name on the second parameter being an fBase_c* and says nothing at all
 * about the third and fourth. The name is left exactly as the config spells it;
 * see the return for what that costs.
 */
fBase_c *dBase_c::Spawn(u32 actorID, fBase_c *parent, int a, int b)
{
    return (fBase_c *)func_02042ffc((void *)actorID, parent);
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_02013edc, 0x02013edc, size 0xc                       */
/* ------------------------------------------------------------------------- */
/* The same forwarder shape, to func_02042fe4 (0x02042fe4). Unnamed, and not an
 * dBase_c member by any evidence -- but it sits INSIDE the class's run,
 * between D0 and Spawn, which is what puts it in this TU. */
extern "C" void func_02013edc(int a, int b, int c)
{
    func_02042fe4(a, b, c);
}

/* ========================================================================= *
 * ROM ordinals 1 and 0 -- the two destructor variants.
 *   _ZN7dBase_cD0Ev  0x02013ea4  size 0x38   (deleting)
 *   _ZN7dBase_cD1Ev  0x02013e80  size 0x24   (complete-object)
 * Written D0 then D1 so the reversal emits them D1, D0 -- the ROM's order.
 *
 * ~dBase_c IS STILL DEFINED INLINE IN include/dBase_c.h, and that is
 * still load-bearing: dScene_c::~dScene_c stores two vptrs and then calls fBase_c's
 * destructor directly, which the compiler can only produce by inlining a visible
 * body, and Stage and every actor destructor below them do the same. Nothing
 * here changes that. What changes is only where the OUT-OF-LINE copies the
 * vtable points at come from.
 *
 * WHY THEY ARE SPELLED OUT INSTEAD OF FORCED. An inline destructor's out-of-line
 * copies are vague-linkage bodies, and something has to pull them into
 * existence. Two ways, and both have a cost this TU cannot pay:
 *
 *   * Let the key function do it. Defining AfterInitResources as a real method
 *     emits _ZTV7dBase_c, and the vtable's slots 16/17 pull D1 and D0 in
 *     for free -- measured, 5/5 MATCH, no scaffold needed. But the emitted
 *     vtable then collides with the gap object's copy and the link aborts.
 *   * A forcing scaffold -- a never-called `p->~dBase_c()`, which is what
 *     src/_ZN7dBase_cD1Ev.cpp uses. objisolate strips that per function,
 *     but a MERGED object cannot be reduced that way: the scaffold is .text no
 *     symbols.txt names, `-nodead` keeps it, and it lands inside this span.
 *
 * So the vague-linkage symbols are spelled directly under their mangled names,
 * which is what the arm9/dActor_c rebuild found. Byte-for-byte the same code the
 * compiler would have produced -- D1 stores the vptr and chains to fBase_c's
 * D2; D0 does that and then returns the object to the actor heap, which is what
 * the inline operator delete on fBase_c compiles to.
 *
 * THE VARIANT ORDER IS STILL THE EVIDENCE that the header's inline definition is
 * right, and this file cannot take that away: the ROM has D1 at 0x02013e80 then
 * D0 at 0x02013ea4 and NO D2 anywhere in the image, which is the vague-linkage
 * shape. An out-of-line definition would have emitted the fixed group D2,D0,D1 --
 * which is exactly what fBase_c, whose destructor IS defined out of line, has
 * at 0x02043d48/0x02043d78/0x02043dbc.
 * ========================================================================= */
extern "C" dBase_c *_ZN7dBase_cD0Ev(dBase_c *self)
{
    *(int *)self = (int)_ZTV7dBase_c;
    _ZN7fBase_cD2Ev(self);
    _ZN6Memory10DeallocateEPvP4Heap(self, data_020a0eac);
    return self;
}

extern "C" dBase_c *_ZN7dBase_cD1Ev(dBase_c *self)
{
    *(int *)self = (int)_ZTV7dBase_c;
    _ZN7fBase_cD2Ev(self);
    return self;
}
