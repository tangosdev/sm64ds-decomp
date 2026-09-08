// run link100 lane PMFB1: THE SEVEN TABLE NAMES THE THIRTEEN FREE
// POINTER-TO-MEMBER DISPATCHERS ASK FOR.
//
// This is the same purely-decorative mismatch hal/pmf3_aliases.cpp answered for
// the indexed-table family, one family along. All thirteen matched TUs on
// port/slice_pmfc.txt share this shape:
//
//     struct C; typedef void (C::*PMF)();
//     struct Entry { PMF pmf; char rest[12]; };     // or one of six siblings
//     extern Entry data_ovNNN_XXXXXXXX[];
//     struct C { char pad[...]; int idx; };
//     extern "C" void f(C *c, int i) { c->idx = i; int j = c->idx;
//                                      (c->*data_ovNNN_XXXXXXXX[j].pmf)(); }
//
// The table is declared at NAMESPACE SCOPE in C++ and not inside extern "C", so
// MSVC mangles the reference
//
//     ?data_ov085_0212fe88@@3PAUEntry@@A
//
// while the mount publishes the same storage at the same address under C
// linkage as _data_ov085_0212fe88 (ov085_syms.c.obj). The reference and the
// definition are the same address and the same bytes; only the decoration
// differs. Editing the matched source to add extern "C" is not available --
// src/ is byte-locked against mwccarm -- so the port does what it already does
// for exactly this class of mismatch. Without these seven the slice does not
// link, and the failure reads as an unresolved external rather than as anything
// about pointers to members. Each mangled name was read out of the TU's own
// /FAs listing (runs/link100/out/PMFB1/zp4_listings/), not guessed.
//
// WHY THIS IS SAFE UNDER port/tools/alternatename_guard.py. The guard's rule is
// that a directive whose LHS acquires a REAL definition has been silently
// defeated. None of these seven can be: the LHS is a C++-mangled name that only
// these thirteen matched TUs ever spell, nothing in the tree defines it, and the
// only definition of the address is the mount's C-linkage symbol on the right.
// LHS and RHS then land in the map at ONE address, which is the guard's healthy
// shape.
//
// WHY THE DISPATCH ITSELF IS SAFE, table by table. Every one of these tables is
// filled with HOST body addresses before the class it belongs to can spawn, by
// a seat that first verifies the mount holds the ROM's own function word and
// ABORTS the binary on a nonzero adjustment word. The adjustment word matters
// twice over: the ROM computes the receiver as `this + (adj >> 1)` with bit 0 of
// adj selecting a VIRTUAL dispatch, while MSVC adds its delta to `this`
// directly, and the two agree at exactly one value, zero.
//
// PORT_HOST_ABI: mwcc pointer-to-member. Table, and the seat that installs host
// addresses into it:
//
//     data_ov002_021097bc   port_pushblock_states_seat
//                           (unmatched/PushBlock_StateDispatch.cpp)
//     data_ov002_0210af2c   port_pathlift_states_seat
//                           (unmatched/PathLift_StateDispatch.cpp)
//     data_ov014_0211476c   port_chain_chomp_states_seat
//                           (unmatched/ChainChomp_States.cpp, twelve halves)
//     data_ov034_02114538   port_wiggler_states_seat
//                           (hal/actor_classes_ov034.cpp, eleven entries)
//     data_ov063_0211efbc   ov63_bringup (hal/actor_classes_ov063.cpp) writes
//                           the four .data SOURCE pairs before the ov063 sinit
//                           copies them into this bss table
//     data_ov071_02122ecc   hal_fill_coffin_vtable
//                           (hal/actor_classes_scuttlebug.cpp) re-seats the four
//                           function words after the sinit has run
//     data_ov085_0212fe88   port_toad_states_seat (hal/actor_overlays.cpp)
//
// THE RECORD STRIDE IS A SEPARATE FIX AND IT IS NOT HERE. Eleven of the thirteen
// read a TWENTY-byte ROM record, and MSVC sizes their Entry at twenty-FOUR,
// because a struct that contains a pointer-to-member gets eight-byte alignment
// even though the pointer-to-member itself is four-aligned. That is corrected
// with a per-TU /Zp4 in port/CMakeLists.txt, block R9d; the derivation and the
// per-row before/after are in port/slice_pmfc.txt.
//
// The ov071 table already carries an unrelated directive of its own
// (_data_ov071_02122ecc_d=_data_ov071_02122ecc, a second dsd spelling of the
// same storage, in hal/actor_classes_scuttlebug.cpp). That one aliases the C
// name to the C name and does not collide with this one.

#pragma comment(linker, "/alternatename:?data_ov002_021097bc@@3PAUEntry@@A=_data_ov002_021097bc")
#pragma comment(linker, "/alternatename:?data_ov002_0210af2c@@3PAUEntry@@A=_data_ov002_0210af2c")
#pragma comment(linker, "/alternatename:?data_ov014_0211476c@@3PAUEntry@@A=_data_ov014_0211476c")
#pragma comment(linker, "/alternatename:?data_ov034_02114538@@3PAUEntry@@A=_data_ov034_02114538")
#pragma comment(linker, "/alternatename:?data_ov063_0211efbc@@3PAUEntry@@A=_data_ov063_0211efbc")
#pragma comment(linker, "/alternatename:?data_ov071_02122ecc@@3PAUEntry@@A=_data_ov071_02122ecc")
#pragma comment(linker, "/alternatename:?data_ov085_0212fe88@@3PAUEntry@@A=_data_ov085_0212fe88")

/* An object with nothing but directives is a legal linker input and its
   .drectve section is read like any other's, but this file also carries one
   real symbol so the object is never mistaken for empty by a tool that reads
   the map rather than the objects. It is called from nowhere on purpose. */
extern "C" int port_pmfc_alias_count(void) { return 7; }
