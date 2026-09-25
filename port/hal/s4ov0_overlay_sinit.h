/* s4ov0_overlay_sinit.h -- force-included (port/CMakeLists.txt, the W31 S4OV0
 * block) into the three matched C++ TUs whose file pointers the cartridge
 * builds in an OVERLAY static initialiser: src/game/actors/d_a_obj_fm_battan.cpp
 * (ov023: data_ov023_02112088 handle 1558, data_ov023_02112080 handle 1559),
 * src/game/actors/d_a_obj_kb1_billboard.cpp (ov044: data_ov044_02111680,
 * handle 1570) and src/game/actors/d_a_star_gate.cpp (ov100:
 * data_ov100_02148934, handle 0x4a1).
 *
 * ON THE CARTRIDGE those four constructors run when their overlay is LOADED,
 * from its __sinit, which is long after the boot worker has built the ov0
 * handle table they look their handles up in. MSVC instead registers them with
 * the CRT, which runs them BEFORE main -- before the heap, the card thread or
 * the table exist -- so with the ROM's own src/func_02018a24.c answering, each
 * one reads data_0209d3b8 while it is still null (measured, run linkfull lane
 * S4OV0: four pre-main lookups, 0x622 / 0x617 / 0x616 / 0x4a1, and those four
 * only). That construction is also a duplicate on this port: the ROM's own
 * sinit bodies already run at each overlay's bring-up, where the ROM runs them
 * (hal/actor_classes_ov023.cpp -> __sinit_ov023_02111aa8, and
 * hal/link21_rows.cpp's __sinit_ov044_02111314 and __sinit_ov100_02147a70).
 *
 * So these TUs' dynamic initialisers go into a section of their own that the
 * CRT does not walk (the documented #pragma init_seg("section") form: objects
 * in a user-named section are not initialised by the runtime). Nothing runs
 * them; the overlay bring-up constructs the objects, as on the DS. They are the
 * only dynamic initialisers these three TUs have (walk_window.map: one
 * ??__E / ??__F pair per object and nothing else).
 */
/* C4075 ("initializers put in unrecognized initialization area") is the
 * compiler saying exactly this: the CRT does not know the section. The
 * documented init_seg example silences it the same way. */
#pragma section("s4ov0sinit$m", read)
#pragma warning(push)
#pragma warning(disable : 4075)
#pragma init_seg("s4ov0sinit$m")
#pragma warning(pop)
