// PORT_HOST_ABI. dScMgHanachan_c's two named traps -- actor id 0x182,
// scene 386, the "Which Wiggler" minigame. Run mg9, lane WIG.
//
// This file exists ONLY for bodies the ROM has and the decomp does not. It
// carries no alias row and no face: this class's src spells no Itanium vtable
// name (both destructors write data_ov006_0213cab8 by its config name, not
// through the shared VT placeholder), and a COFF short-name scan for _VT and
// _HEAP over this seat's objects finds nothing, so there is no per-source
// rename to make. If a later link asks for a name this file is where it goes.
//
// ---- THE TWO FLOORS, AND HOW THEY WERE FOUND ------------------------------
//
// Not by the vtable axis. port/mg_fanout_costs.txt section 3 records this class
// as 7 overrides / 6 markers / ZERO nosrc, and that column is right: all seven
// override bodies have a matched src TU. Section 13's CORRECTION 3 says the
// check that finds a real floor is a delinks join over every CALLEE of every
// override, not over the state addresses, and that is what turned these up. Of
// the 64 ov006 functions this class's relocation closure reaches, exactly two
// have a config symbol, NO delink block in
// config/arm9/overlays/ov006/delinks.txt, and no src file in either extension
// in any module:
//
//   func_ov006_020ea914  0x324  ON THE RENDER PATH, and it is the same shape
//       section 15 measured for Memory Master's card draw. Slot 9 Render calls
//       it UNCONDITIONALLY at 0x020ed0c8 -- `add r0,r4,#0x4000 / ldr r0,[r0,
//       #0xf60] / bl 0x020ea914`, receiver only, no second argument -- and that
//       is its ONLY call site: exactly one arm_call relocation in all of ov006
//       reaches 0x020ea914. Whatever this body draws, nothing else in the class
//       draws it.
//
//   func_ov006_020ec4dc  0x20c  ONE OF FOUR WIGGLER SET-UP VARIANTS, and this
//       is the useful half of the finding. src/func_ov006_020ecdb8.c ends in a
//       four-way switch on data_ov006_02141fd8:
//           case 0 -> func_ov006_020ecba4    matched
//           case 1 -> func_ov006_020ec9c0    matched
//           case 2 -> func_ov006_020ec84c    matched
//           case 3/default -> func_ov006_020ec4dc   THIS, and only this, is a
//                                                   decomp gap
//       Its only call site is 0x020eceb0, `mov r0,r6 / mov r1,r5 / bl` --
//       receiver and ONE argument, which is why the trap below declares two
//       parameters rather than cleaning one. So three of the four wiggler kinds
//       are whole and the fourth is missing, and the class only reaches the
//       fourth when that selector reads 3.
//
// BOTH TRAPS COUNT THEMSELVES AND BOTH ARE REPORTED WHETHER OR NOT THEY FIRE.
// A silent zero and an absent instrument look the same in a log, and the two
// zeros here mean different things: a zero on 0x020ea914 would mean Render never
// ran, and a zero on 0x020ec4dc means the selector never read 3. The seat's
// report says which.
//
// NEITHER TRAP GUESSES. port/tools/inferred_stub_guard exists to refuse a
// plausible body, and a render routine and a set-up routine are exactly the
// shapes where a plausible body is worst: it would draw something, and nobody
// would know it was invented. Both count and return.

#include <cstdio>

static unsigned g_wig_trap_020ea914;
static unsigned g_wig_trap_020ec4dc;

extern "C" {

/* func_ov006_020ea914(void *) -- slot 9 Render's sixth call, the class's only
   draw of whatever sits at scene+0x4f60. */
void func_ov006_020ea914(void *p)
{
    (void)p;
    ++g_wig_trap_020ea914;
}

/* func_ov006_020ec4dc(void *, int) -- wiggler set-up variant 3. TWO
   parameters, read off the ROM's own call site and not from the src
   declaration, though the two agree here. */
void func_ov006_020ec4dc(void *self, int arg)
{
    (void)self;
    (void)arg;
    ++g_wig_trap_020ec4dc;
}

void port_mg_wiggler_trap_counts(unsigned *render, unsigned *setup)
{
    if (render) *render = g_wig_trap_020ea914;
    if (setup)  *setup  = g_wig_trap_020ec4dc;
}

}  /* extern "C" */
