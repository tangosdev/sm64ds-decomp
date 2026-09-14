/* NAME BRIDGES for the seats lane SEATS2 added, run link100 wave 9b.
 *
 * Seating src/actors/daPkn_c.cpp brings seven references to ROM DATA that the
 * translation unit declares as ordinary C++ externs, with no linkage
 * specification:
 *
 *     extern PknSharedFile data_ov084_02130df4;
 *
 * mwccarm emits the flat ROM name for that, because the decomp compiles the
 * whole file as C++ with C-linkage ROM symbols; MSVC decorates it,
 * ?data_ov084_02130df4@@3UPknSharedFile@@A, and the port's storage is the flat
 * _data_ov084_02130df4 the mount defines. Nothing in the link answers the
 * decorated spelling, so each one would be a fresh unresolved row the moment
 * the seat lands.
 *
 * AN /alternatename IS THE RIGHT ANSWER HERE AND NOT A SHORTCUT. It is a NAME
 * bridge and never an ABI bridge, which is the whole reason the method rows on
 * this wall need faces instead: a member's reference is __cdecl with the
 * receiver pushed and its definition takes the receiver in ECX, so an alias
 * would link and the receiver would be garbage. These rows are DATA. There is
 * no receiver, no calling convention and no argument list -- the two spellings
 * name one address in one mount -- so the alias is exact, and it is the same
 * row shape the tree already ships for this class of name in
 * port/hal/except_faces.cpp, port/hal/actor_faces_bob.cpp and
 * port/hal/pmfc_aliases.cpp.
 *
 * Each right-hand side was checked DEFINED in walk_window's own link inputs
 * before the row was written (8663 objects, 48406 defined externals), and each
 * left-hand side was checked NOT defined, which is what alternatename_guard
 * refuses on: a defined left-hand side makes the directive inert and nothing
 * else says so.
 *
 * The three data names the same seat needs that ALREADY have live rows are not
 * repeated here: ?data_ov014_0211476c@@3PAUEntry@@A in
 * port/hal/pmfc_aliases.cpp, and ?data_ov014_02114970@@3DA and
 * ?data_ov014_02114980@@3DA in port/hal/bob_enemy_bridges.cpp.
 */

/* ov002, the shared file-pointer block PIRANHA_PLANT reads through. */
#pragma comment(linker, "/alternatename:?data_ov002_0210da38@@3UPknSharedFile@@A=_data_ov002_0210da38")

/* ov084's own descriptors. 021302f4 is the array of SharedFilePtr pointers;
 * the five at 02130df4..02130e24 are the class's file records. */
#pragma comment(linker, "/alternatename:?data_ov084_021302f4@@3PAPAUSharedFilePtr@@A=_data_ov084_021302f4")
#pragma comment(linker, "/alternatename:?data_ov084_02130df4@@3UPknSharedFile@@A=_data_ov084_02130df4")
#pragma comment(linker, "/alternatename:?data_ov084_02130dfc@@3UPknSharedFile@@A=_data_ov084_02130dfc")
#pragma comment(linker, "/alternatename:?data_ov084_02130e0c@@3UPknSharedFile@@A=_data_ov084_02130e0c")
#pragma comment(linker, "/alternatename:?data_ov084_02130e14@@3UPknSharedFile@@A=_data_ov084_02130e14")
#pragma comment(linker, "/alternatename:?data_ov084_02130e24@@3UPknSharedFile@@A=_data_ov084_02130e24")


/* GATE 2, src/actors/daGmch_c.cpp (ov081 MONEYBAG/daGmch_c). Same shape and
 * same rule as the rows above: ROM data the translation unit declares as an
 * ordinary C++ extern, whose storage the port hosts under the flat name.
 * data_0209e650 gets TWO rows because two headers this TU includes declare
 * that one address with different types, so a single row would close half
 * the references and leave the other half on the wall (out/SEATS2/bugs.md
 * section 2). The other decorated names this seat needs already have live
 * rows elsewhere in port/hal/ and are not repeated here. */
#pragma comment(linker, "/alternatename:?data_0209e650@@3PAHA=_data_0209e650")
#pragma comment(linker, "/alternatename:?data_ov081_021280d8@@3PAPAXA=_data_ov081_021280d8")
#pragma comment(linker, "/alternatename:?data_ov081_02128ec4@@3PAPAXA=_data_ov081_02128ec4")
#pragma comment(linker, "/alternatename:?data_ov081_02128ec4@@3UBca2@@A=_data_ov081_02128ec4")
#pragma comment(linker, "/alternatename:?data_ov081_02128ecc@@3PAHA=_data_ov081_02128ecc")
#pragma comment(linker, "/alternatename:?data_ov081_02128ed4@@3PAXA=_data_ov081_02128ed4")
#pragma comment(linker, "/alternatename:?data_ov081_02128edc@@3PAHA=_data_ov081_02128edc")
#pragma comment(linker, "/alternatename:?data_ov081_02128ee4@@3UBca2@@A=_data_ov081_02128ee4")
#pragma comment(linker, "/alternatename:?data_ov081_02128f40@@3DA=_data_ov081_02128f40")

/* A translation unit with no code in it still has to have something to
 * compile, and the pragmas above are directives rather than definitions. */
extern "C" int port_seats2_name_bridges_present;
int port_seats2_name_bridges_present = 1;
