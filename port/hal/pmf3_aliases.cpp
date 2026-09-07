// run link100 lane PMF3, batch 2: THE SEVEN NAMES THE INDEXED-TABLE FAMILY
// ASKS FOR, AND WHY THEY ARE ALIASES RATHER THAN A SOURCE CHANGE.
//
// Fourteen matched TUs share one shape:
//
//     struct C; typedef void (C::*PMF)();
//     struct Entry { PMF pmf[2]; };
//     extern Entry data_ovNNN_XXXXXXXX[];
//     struct C { char pad[...]; int idx; };
//     void f(C *c, int i) { c->idx = i; int j = c->idx;
//                           (c->*data_ovNNN_XXXXXXXX[j].pmf[0])(); }
//
// With /vmg /vmm target-wide (block R8), MSVC's pointer-to-member IS the ROM's
// 8-byte {function, delta} pair, so Entry is 16 bytes and the emitted body
// strides `shl edx, 4` -- the ROM's own stride -- and TAIL JUMPS through the
// record's function word with `this + delta` in ecx. Measured on 2026-09-07
// under the port's own flags for all eight tables; the listings are in
// runs/link100/out/PMF3/.
//
// WHAT IS LEFT IS PURELY A NAME. Those TUs declare the table at namespace
// scope in C++ and NOT inside extern "C", so MSVC mangles the reference:
//
//     ?data_ov084_02130dc4@@3PAUEntry@@A
//
// while the mount publishes the table under C linkage as _data_ov084_02130dc4.
// The reference and the definition are the same address and the same bytes;
// only the decoration differs. Editing the matched source to add extern "C"
// is not available -- src/ is byte-locked against mwccarm -- so the port does
// what it already does for exactly this class of mismatch: an /alternatename,
// the shape hal/actor_base_tables_ov002.cpp and hal/actor_classes_bob_enemy.cpp
// use for their own SharedFilePtr tables.
//
// WHY THIS IS SAFE UNDER port/tools/alternatename_guard.py. The guard's rule
// is that a directive whose LHS acquires a REAL definition has been defeated
// and silently rebinds. None of these seven can be: the LHS is a C++-mangled
// name that only these matched TUs ever spell, nothing in the tree defines it,
// and the only definition of the address is the mount's C-linkage symbol on
// the right. The guard's healthy shape -- LHS and RHS present in the map at
// the SAME address -- is what these produce.
//
// WHY THE DISPATCH IS SAFE. Every one of the seven tables is filled by a seat
// that verifies the mount holds the ROM's own function address, ABORTS the
// binary on a nonzero adjustment word, and then rewrites the function word
// with a HOST body's address, all before the class can spawn:
//
//     data_ov002_0210e00c   port_exclamation_switch_states_seat
//     data_ov002_0210e084   port_sign_post_states_seat
//     data_ov064_0211c98c   port_treasure_chest_states_seat
//     data_ov074_021230f8   port_ov074_states_seat
//     data_ov084_02130dc4   port_bob_omb_buddy_states_seat (source AND table)
//     data_ov098_0213c878   port_crate_states_seat
//     data_ov102_0214e890   port_question_block_states_seat
//
// and every source pair those seats copy was re-read out of the raw overlay
// images with its relocation on 2026-09-07: seventy-eight pairs, every
// adjustment word ROM zero (runs/link100/out/PMF3/deltas_static_b2.txt).
//
// THE EIGHTH TABLE IS DELIBERATELY ABSENT. data_ov002_02110a5c (YOSHI_EGG) has
// the same shape and the same zero deltas, but NOTHING RE-SEATS IT: its host
// copy is an address switch that reads the ROM's DS word and calls the matching
// host body by name, so the table still holds DS code addresses at run time and
// a matched dispatcher would jump into reserved memory. Aliasing it would buy
// nothing and would leave a directive whose LHS is never referenced. See
// port/slice_pmf3.txt.

/* This first one is also spelled in hal/actor_faces_bob.cpp:163, which
   reached the same table from the face side before this lane existed. Two
   identical directives are not a conflict -- the linker takes the same
   substitution twice and alternatename_guard scores both as fired -- and it
   is kept here so the seven tables this slice rests on read as one set
   rather than six plus a dependency on another lane's file. */
#pragma comment(linker, "/alternatename:?data_ov002_0210e00c@@3PAUEntry@@A=_data_ov002_0210e00c")
#pragma comment(linker, "/alternatename:?data_ov002_0210e084@@3PAUEntry@@A=_data_ov002_0210e084")
#pragma comment(linker, "/alternatename:?data_ov064_0211c98c@@3PAUEntry@@A=_data_ov064_0211c98c")
#pragma comment(linker, "/alternatename:?data_ov074_021230f8@@3PAUEntry@@A=_data_ov074_021230f8")
#pragma comment(linker, "/alternatename:?data_ov084_02130dc4@@3PAUEntry@@A=_data_ov084_02130dc4")
#pragma comment(linker, "/alternatename:?data_ov098_0213c878@@3PAUEntry@@A=_data_ov098_0213c878")
#pragma comment(linker, "/alternatename:?data_ov102_0214e890@@3PAUEntry@@A=_data_ov102_0214e890")

/* An object with nothing but directives is a legal linker input and its
   .drectve section is read like any other's, but this file also carries one
   real symbol so the object is never mistaken for empty by a tool that reads
   the map rather than the objs. It is called from nowhere on purpose. */
extern "C" int port_pmf3_alias_count(void) { return 7; }
