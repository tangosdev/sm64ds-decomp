//cpp
/* leftovers:
 * func_020147bc keeps its address name: a print helper shared with
 * nds_print.c and nds_printt.cpp, which keep their own declarations and call
 * sites.
 * data_0208e500 keeps its address name: ROM data resolved from
 * config/arm9/symbols.txt, defined by no TU.
 */
/* Positional collision cylinders (arm9 .text 0x020147bc..0x0201489c):
 * dCcPos_c, the dCc_c leaf that carries its own position -- a fixed
 * cylinder, not bound to an actor, which is why GetOwnerID is a constant
 * 0 and GetPos returns the member at +0x30.
 * mwccarm emits .text in reverse source order, so the definitions below
 * run ROM-descending.
 *
 * ~dCcPos_c() is the key function: the vtable _ZTV8dCcPos_c (0x0208e6bc)
 * emits from this TU with slots [D1 0x02014854, D0 0x02014828, GetPos
 * 0x02014820, GetOwnerID 0x02014818]. The class's RTTI records exist in
 * the cartridge -- _ZTS8dCcPos_c at 0x0208e674 and _ZTI8dCcPos_c at
 * 0x0208e68c, a __si_class_type_info on dCc_c -- so the records
 * externalize as deadstrip-data at their configured homes; C2 and D2 are
 * the homeless variants, licensed compiler-only.
 */
#include "dCcPos_c.h"

extern "C" {
/* local extern: ROM data no TU defines and no header declares -- the
   font-base halfword the absorbed print helper below adds the glyph index to. */
extern unsigned short data_0208e500;
}

// @symbol _ZN8dCcPos_cC1Ev
/* pos is left uninitialised: the body is empty and the compiler emits the
   dCc_c base step and the vptr store from the class declaration alone. */
dCcPos_c::dCcPos_c()
{
}

// @symbol _ZN8dCcPos_cD1Ev
/* Empty body: the vtable store and the dCc_c subobject call are what
   `struct dCcPos_c : dCc_c` and `virtual ~dCcPos_c()` already mean; the
   one member is a Vector3, which has no destructor. D0 adds the class
   operator delete tail. */
dCcPos_c::~dCcPos_c()
{
}

// @symbol _ZN8dCcPos_c6GetPosEv
/* The whole body is the reference itself -- `add r0, r0, #0x30'. */
Vector3 &dCcPos_c::GetPos()
{
    return pos;
}

// @symbol _ZN8dCcPos_c10GetOwnerIDEv
/* A positional cylinder has no owning dActor_c: constant 0 where
   dCcAc_c's slot reads through its owner pointer. */
u32 dCcPos_c::GetOwnerID()
{
    return 0;
}

// @symbol _ZN8dCcPos_c4InitERK7Vector3iijj
/* Scalar Fix12i: a by-value Fix12<int> is the class type wall 6az homes
   on the stack. The reference is the same address the pointer spelling
   passed. */
void dCcPos_c::Init(const Vector3 &pos, Fix12i radius, Fix12i height, u32 flags, u32 vulnFlags) {
    this->pos.x = pos.x;
    this->pos.y = pos.y;
    this->pos.z = pos.z;
    dCc_c::Init(radius, height, flags, vulnFlags);
}

/* ROM ordinal 0 -- the run's backward neighbour, absorbed (reverse emission
 * puts the lowest address last). A print helper shared with nds_print.c and
 * nds_printt.cpp, which keep their own matching extern declarations and call
 * sites; only the definition moves. Body as recovered. */

// @symbol func_020147bc
extern "C" void func_020147bc(unsigned short *dst, int glyph)
{
    *dst = (unsigned short)(data_0208e500 + glyph);
}
