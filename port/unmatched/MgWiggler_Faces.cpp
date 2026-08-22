// PORT_HOST_ABI. dScMgHanachan_c's two named traps, three alias rows and one
// argument face -- actor id 0x182, scene 386, the "Which Wiggler" minigame.
// Run mg9, lane WIG.
//
// NO VT OR HEAP RENAME IS OWED. This class's src spells no Itanium vtable name:
// both destructors write data_ov006_0213cab8 by its config name rather than
// through the shared VT placeholder hal/cxx_aliases.cpp binds to the ov002
// ENEMY table, and the D0 loads the heap through data_020a0eac. A COFF
// short-name scan for _VT and _HEAP over this seat's built objects is the proof
// rather than this sentence, and it finds nothing.
//
// ---- WHAT THE FIRST LINK ASKED FOR ----------------------------------------
//
// FIVE unresolved externals across four objects, one wave, and every one of
// them is an ordinary name-spelling defect of the kinds
// port/mg_fanout_costs.txt section 4 counts twenty-two of. None is a
// pointer-to-member symbol, which is what section 4 predicts for a class whose
// whole wall is the FIELD form: there is no table global for a link to fail on.
//
//   ?data_ov006_0213c96c@@3UPair@@A   src/func_ov006_020eb8f0.cpp declares its
//   ?data_ov006_0213c9bc@@3UPair@@A   three .data pairs OUTSIDE its extern "C"
//   ?data_ov006_0213c9c4@@3UPair@@A   block, so they mangle as C++ globals and
//                                     the ov006 mount's plain C definitions do
//                                     not satisfy them. The link even prints
//                                     the C names as hints.
//
//     THEY ARE ALIAS-SAFE AND THE RULEBOOK IS WHAT SAYS SO. Section 4's
//     corollary: a pair whose consumer spells it as TWO INTS is safe as an
//     alias; a pair whose consumer names a MEMBER-POINTER TYPE needs a host
//     copy, whether it is called or only copied. That TU's declaration is
//     `struct Pair { int v[2]; };` -- eight bytes on both machines -- and it
//     only compares and copies. So three /alternatename rows are correct here
//     and a host copy would be work for nothing.
//
//     0x0213c96c IS THE IDLE SENTINEL, which is the other half of why the pair
//     must keep the ROM's own words: that TU asks "is this slot idle" by VALUE
//     (`p->v[0] == g->v[0] && (p->v[1] == g->v[1] || p->v[0] == 0)`), and a
//     host address never equals a DS address.
//
//   _func_0203adec   src/func_ov006_020ecb80.c calls arm9 0x0203adec under an
//                    ADDRESS-SHAPED name that exists in no config.
//                    config/arm9/symbols.txt names that address
//                    _Z14ApproachLinearRsss, and src/_Z14ApproachLinearRsss.cpp
//                    defines it as the C++ overload ApproachLinear(short&,
//                    short, short). AN ALIAS IS NOT USED HERE. The two spellings
//                    differ in their third parameter -- int against short -- and
//                    on __cdecl that happens to work, but a face states the
//                    mapping instead of relying on it, and it is one line.
//
//   _func_0203d5dc   NOT a spelling defect at all: a real arm9 body, matched,
//                    with a src TU that was in no slice. It is a slice line in
//                    port/slice_wig.txt and not a face.
//
// ---- THE TWO FLOORS ARE CLOSED, AND THIS FILE NO LONGER TRAPS THEM --------
//
// Run mg10, lane F386. Both bodies now have a src TU, both are in
// port/slice_wig.txt, and the two counting traps that used to stand here are
// gone. The header is kept because how they were FOUND is the reusable part:
// not by the vtable axis -- port/mg_fanout_costs.txt section 3 records this
// class as 7 overrides / 6 markers / ZERO nosrc and that column is right -- but
// by section 13's CORRECTION 3, a delinks join over every CALLEE of every
// override. Of the 64 ov006 functions this class's relocation closure reaches,
// exactly two had a config symbol, NO delink block and no src file in either
// extension in any module:
//
//   func_ov006_020ea914  0x324  ON THE RENDER PATH, the same shape section 15
//       measured for Memory Master's card draw. Slot 9 Render calls it
//       UNCONDITIONALLY at 0x020ed0c8 -- `add r0,r4,#0x4000 / ldr r0,[r0,
//       #0xf60] / bl 0x020ea914`, receiver only, no second argument -- and that
//       is its ONLY call site: exactly one arm_call relocation in all of ov006
//       reaches 0x020ea914. Whatever this body draws, nothing else in the class
//       draws it, and the trap's 1200-of-1200 said the class was drawing it
//       into a counter on every rendered frame.
//       NOW: src/func_ov006_020ea914.c, NONMATCHING with 165 of 201 words
//       identical and one register-allocation cause written up in the file. It
//       is the question picture -- one wiggler at a fixed pose, five body
//       segments through OAM::Render and the face sprite on segment 0.
//
//   func_ov006_020ec4dc  0x20c  ONE OF FOUR WIGGLER SET-UP VARIANTS.
//       src/func_ov006_020ecdb8.c ends in a four-way switch on
//       data_ov006_02141fd8:
//           case 0 -> func_ov006_020ecba4    matched
//           case 1 -> func_ov006_020ec9c0    matched
//           case 2 -> func_ov006_020ec84c    matched
//           case 3/default -> func_ov006_020ec4dc   this
//       Its only call site is 0x020eceb0, `mov r0,r6 / mov r1,r5 / bl` --
//       receiver and ONE argument, which the trap read off the ROM rather than
//       off the src declaration, and the seated body takes the same two.
//       NOW: src/func_ov006_020ec4dc.c, MATCHED at mwccarm 2004/b56 with strict
//       relocs and carrying a delink block. It lays out the fifteen-wiggler
//       grid the difficulty ladder reaches at clear count 8.
//
// NO REPLACEMENT INSTRUMENT WAS INVENTED. A trap counts itself because a
// trapped body cannot be observed any other way; a seated one can. 020ea914 is
// Render's unconditional and only callee, so the seat's slot 9 hit count IS its
// call count, and 020ec4dc runs exactly when the dealt kind is 3, which the
// seat's round line already prints. Both facts come off measurements the run
// already takes, so port_mg_wiggler_trap_counts is removed rather than
// reimplemented against something weaker.

/* <cstdio> was here for the two traps' counters and nothing left in this file
   prints. */

/* The three alias rows, section "WHAT THE FIRST LINK ASKED FOR" above. */
#pragma comment(linker, "/alternatename:?data_ov006_0213c96c@@3UPair@@A=_data_ov006_0213c96c")
#pragma comment(linker, "/alternatename:?data_ov006_0213c9bc@@3UPair@@A=_data_ov006_0213c9bc")
#pragma comment(linker, "/alternatename:?data_ov006_0213c9c4@@3UPair@@A=_data_ov006_0213c9c4")

/* The argument face. src/_Z14ApproachLinearRsss.cpp defines this overload as an
   ordinary C++ function; hal/method_faces.cpp already declares it the same way
   for its own _Z15ApproachLinear2Rsss row. */
extern "C++" int ApproachLinear(short &x, short target, short step);

extern "C" {

/* func_0203adec is arm9's _Z14ApproachLinearRsss under the address-shaped name
   src/func_ov006_020ecb80.c spells. The src declares the step as an int and the
   ROM body reads it as one register either way; the face narrows it explicitly
   rather than leaving the widths to __cdecl. */
void func_0203adec(short *x, short target, int step)
{
    ApproachLinear(*x, target, (short)step);
}

}  /* extern "C" */
