/* port/unmatched/Seats3_NameBridges.cpp -- lane SEATS3, run link100 wave 9c.
 *
 * EIGHT DECORATED ROM DATA NAMES, one seat's worth. src/actors/daGmch_c.cpp
 * declares each of these ov081 data symbols as an ordinary C++ extern, inside
 * or below a header that carries no linkage-specification, so MSVC decorates
 * the reference while the port's mount (port/ov081_syms.txt through
 * hal/actor_vtables.cpp) emits the storage under the flat C name. An
 * /alternatename is a NAME bridge and nothing else, which is why it is
 * admissible for a datum and not for a receiver-shape mismatch: a datum has no
 * calling convention. The tree's own precedent for the shape is
 * port/hal/actor_classes.cpp:870.
 *
 * MEASURED, not predicted. The generated copy of the seat was compiled with
 * the build's own flags and its undefined externals were read against the
 * 49,647 defined externals of walk_window's 8,662 link inputs and against the
 * 241-row wall. Twenty-seven names came back outside both sets; nineteen of
 * them already have a live /alternatename or come out of the CRT under -MT
 * (??_7type_info@@6B@, ___CxxFrameHandler3, ___security_cookie,
 * @__security_check_cookie@4 and __allmul, checked on a fixture that leaves
 * them UNDEF in the object and links rc 0). These eight are the remainder, and
 * every right-hand side below is DEFINED in this link today.
 *
 * TWO ADDRESSES GET TWO ROWS EACH. data_ov081_02128ec4 and data_ov081_02128ee4
 * are each declared with two different types by two headers this translation
 * unit includes, and neither declaration says extern "C", so MSVC emits two
 * decorated spellings for one piece of storage. A single row would close half
 * the references and leave the other half on the wall. That shape was first
 * written down for data_0209e650 in out/SEATS2/bugs.md section 2; this is the
 * second and third instance of it.
 */

#pragma comment(linker, "/alternatename:?data_ov081_021280d8@@3PAPAXA=_data_ov081_021280d8")
#pragma comment(linker, "/alternatename:?data_ov081_02128ec4@@3PAPAXA=_data_ov081_02128ec4")
#pragma comment(linker, "/alternatename:?data_ov081_02128ec4@@3UBca2@@A=_data_ov081_02128ec4")
#pragma comment(linker, "/alternatename:?data_ov081_02128ecc@@3PAHA=_data_ov081_02128ecc")
#pragma comment(linker, "/alternatename:?data_ov081_02128ed4@@3PAXA=_data_ov081_02128ed4")
#pragma comment(linker, "/alternatename:?data_ov081_02128edc@@3PAHA=_data_ov081_02128edc")
#pragma comment(linker, "/alternatename:?data_ov081_02128ee4@@3UBca2@@A=_data_ov081_02128ee4")
#pragma comment(linker, "/alternatename:?data_ov081_02128f40@@3DA=_data_ov081_02128f40")


/* TWENTY-FIVE MORE, src/actors/daMip_c.cpp (ov085 MIPS the rabbit), measured
 * the same way and on the same day. This translation unit is the extreme case
 * of the two-spellings shape: SIX ov085 addresses are each declared with two
 * or three different types by the members that read them, and 021305c0 alone
 * reaches the linker as PAHA, PAPAXA and UdaMip_G. Every right-hand side below
 * is DEFINED in this link (port/ov085_syms.txt through hal/actor_vtables.cpp),
 * and none of the left-hand sides is defined anywhere, which is what
 * alternatename_guard checks.
 *
 * These are DATA ROWS ONLY. The same directive would close a function row
 * whose decorated twin is defined, and this lane wrote none of those: facegen
 * derives exactly those rows out of port/faces_sync.txt, and an alias whose
 * left-hand side a face later defines is DEFEATED and alternatename_guard
 * refuses the build.
 */

#pragma comment(linker, "/alternatename:?data_02092120@@3CA=_data_02092120")
#pragma comment(linker, "/alternatename:?data_0209f33c@@3PAXA=_data_0209f33c")
#pragma comment(linker, "/alternatename:?data_ov085_021305b0@@3PAPAHA=_data_ov085_021305b0")
#pragma comment(linker, "/alternatename:?data_ov085_021305b0@@3PAPAXA=_data_ov085_021305b0")
#pragma comment(linker, "/alternatename:?data_ov085_021305b8@@3PAHA=_data_ov085_021305b8")
#pragma comment(linker, "/alternatename:?data_ov085_021305c0@@3PAHA=_data_ov085_021305c0")
#pragma comment(linker, "/alternatename:?data_ov085_021305c0@@3PAPAXA=_data_ov085_021305c0")
#pragma comment(linker, "/alternatename:?data_ov085_021305c0@@3UdaMip_G@@A=_data_ov085_021305c0")
#pragma comment(linker, "/alternatename:?data_ov085_021305c8@@3PAPAHA=_data_ov085_021305c8")
#pragma comment(linker, "/alternatename:?data_ov085_021305c8@@3PAPAXA=_data_ov085_021305c8")
#pragma comment(linker, "/alternatename:?data_ov085_021305d0@@3PAPAXA=_data_ov085_021305d0")
#pragma comment(linker, "/alternatename:?data_ov085_0213066c@@3PAXA=_data_ov085_0213066c")
#pragma comment(linker, "/alternatename:?data_ov085_0213067c@@3PAHA=_data_ov085_0213067c")
#pragma comment(linker, "/alternatename:?data_ov085_0213068c@@3DA=_data_ov085_0213068c")
#pragma comment(linker, "/alternatename:?data_ov085_0213068c@@3PADA=_data_ov085_0213068c")
#pragma comment(linker, "/alternatename:?data_ov085_0213069c@@3PADA=_data_ov085_0213069c")
#pragma comment(linker, "/alternatename:?data_ov085_021306ac@@3DA=_data_ov085_021306ac")
#pragma comment(linker, "/alternatename:?data_ov085_021306ac@@3PAHA=_data_ov085_021306ac")
#pragma comment(linker, "/alternatename:?data_ov085_021306bc@@3DA=_data_ov085_021306bc")
#pragma comment(linker, "/alternatename:?data_ov085_021306bc@@3PADA=_data_ov085_021306bc")
#pragma comment(linker, "/alternatename:?data_ov085_021306bc@@3PAHA=_data_ov085_021306bc")
#pragma comment(linker, "/alternatename:?data_ov085_021306cc@@3PADA=_data_ov085_021306cc")
#pragma comment(linker, "/alternatename:?data_ov085_021306cc@@3PAHA=_data_ov085_021306cc")
#pragma comment(linker, "/alternatename:?data_ov085_021306dc@@3DA=_data_ov085_021306dc")
#pragma comment(linker, "/alternatename:?data_ov085_021306dc@@3HA=_data_ov085_021306dc")

/* TWO FLAT ROM NAMES THAT ARE A SECOND SPELLING OF ONE ADDRESS, and the only
 * rows in this file that are not about a seat. These are two of the twelve
 * flat C / flat ROM rows lane SEATS3 was given, and they are closed by a name
 * bridge rather than by seating anything.
 *
 * src/game/actors/d_a_obj_wc_mizu.cpp releases two SharedFilePtrs it declares
 * as `extern SharedFilePtr daObjWc_Obj07_c_ModelFile;` and `..._ClsnFile`,
 * inside an extern "C" region, so the reference reaches the linker flat. The
 * storage is real and it is in this link: config/arm9/overlays/ov029/
 * symbols.txt gives both names as BSS at addresses the mount already emits
 * under its data_ spelling --
 *
 *     daObjWc_Obj07_c_ModelFile   0x021142fc   == data_ov029_021142fc
 *     daObjWc_Obj07_c_ClsnFile    0x02114304   == data_ov029_02114304
 *
 * -- one symbol table row each, same address, and each 8 bytes wide, which is
 * the ROM span (0x021142fc..0x02114304 and 0x02114304..0x0211430c) and the
 * size a SharedFilePtr needs. Checked on the generated mount rather than
 * assumed: build/port/host-src/ov029_syms.c declares each as u8[8] in the
 * packed ROM-order region with its own offset self-check. A datum has no
 * calling convention, so this is a pure name bridge.
 */

#pragma comment(linker, "/alternatename:_daObjWc_Obj07_c_ModelFile=_data_ov029_021142fc")
#pragma comment(linker, "/alternatename:_daObjWc_Obj07_c_ClsnFile=_data_ov029_02114304")

/* A translation unit with no code in it still has to have something to
 * compile, and the pragmas above are directives rather than definitions. */
extern "C" int port_seats3_name_bridges_present;
int port_seats3_name_bridges_present = 1;
