/* PORT_HOST_ABI.  dScMgTrampoline_c's name-spelling repairs and its one hosted
 * arm9 BSS global.  Run mg11, lane TTI.
 *
 * Nothing here is a decompilation and nothing here changes behaviour.  Every
 * row is a C-named ROM symbol that some TU in this class's closure declares at
 * C++ LINKAGE, so MSVC mangles the reference and the plain C definition the
 * mount already provides does not satisfy it.  link.exe prints the hint itself
 * on each one ("symbols that are defined and could potentially match:
 * _data_ov006_...").  port/tools/alternatename_guard.py is the check that a row
 * is doing something rather than being shadowed by a real definition, and it
 * runs post-link on every map.
 *
 * A DUPLICATE ROW IS HARMLESS.  An /alternatename with the same left and right
 * sides declared twice is deduplicated by the linker, so a sibling lane that
 * reaches the same body will find these standing and needs no coordination.
 *
 * ---- 1. THE THIRTEEN SharedFilePtr ROWS ----------------------------------
 *
 * src/func_ov006_020ccfc8.cpp is the ELEMENT DESTRUCTOR of the four
 * dMgTrmpln3DMario_c records at this+0x500c (port/slice_tti.txt section 8).
 * It is a //cpp TU that declares thirteen of ov006's SharedFilePtr .bss
 * objects as `extern SharedFilePtr data_ov006_...;`, so each mangles as
 * ?data_ov006_...@@3USharedFilePtr@@A.  The mount defines all thirteen as
 * plain C names.
 *
 * ---- 2. THE SIXTEEN int AND short ROWS -----------------------------------
 *
 * Same TU and its neighbours, same shape, different types: fourteen `int`
 * (?...@@3HA) and three `short` (?...@@3FA).  data_ov006_0212e044/48/4c are the
 * three halfwords chain link 0 (src/func_ov006_02121d64.cpp) and the state
 * setter src/func_ov006_02121f04.c seed the object's +0x5db0..+0x5db6 block
 * from.
 *
 * TWO OF THE int ROWS ARE VTABLES AND NOT DATA, which is worth naming rather
 * than leaving in a list of sixteen: ?data_ov006_0213b2c4@@3HA is
 * dMgTrmpln3DMario_c's own five-slot table -- the ELEMENT VTABLE this seat
 * fills -- and ?data_ov006_0213b3c4@@3HA is the neighbouring class's, which the
 * same element destructor also names.
 *
 * ---- 3. THREE BARE NAMES THAT ARE NOT THE SAME SYMBOL AT ALL -------------
 *
 * These three are stronger than a linkage mismatch: the src spells a name that
 * NO config symbol carries, and the ROM address behind it belongs to something
 * with a different name.  Each was resolved from the literal pool rather than
 * from the spelling.
 *
 *   _ZTV18dMgTrmpln3DMario_c   src/func_ov006_020cd12c.c's element constructor
 *       writes it into word 0 of each record.  The pool word at 0x020cd154
 *       reads 0x0213b2c4 (config: `from:0x020cd154 kind:load to:0x0213b2c4`),
 *       so the vtable IS data_ov006_0213b2c4 and the name is the class's, taken
 *       from the ROM's own type_info: 0x0213b2c0 -> 0x0213b244 +4 -> 0x0213b270
 *       = "18dMgTrmpln3DMario_c", whose base 0x0213b19c names
 *       "19dMgTrmpObjAdapter_c".
 *
 *   func_020beb74   src/func_ov006_020e7124.c uses it as `extern int
 *       func_020beb74[]` and writes func_020beb74[1] and data_0209d4a8.  It is
 *       not a function: config/arm9/overlays/ov004/symbols.txt has
 *       `data_ov004_020beb74 kind:bss addr:0x020beb74`.  The src's `func_`
 *       prefix is a decompiler default applied to an address it could not
 *       classify, and the alias points it at the mount's bss object.
 *
 *   Scene_AfterRender   src/_ZN17MgBounceAndPounce11AfterRenderEj.cpp tail-
 *       calls it.  The ROM body 0x020e700c is `ldr ip,[pc,#4] / bx ip` and its
 *       one pool word at 0x020e703c reads 0x0202e398 (config:
 *       `from:0x020e703c kind:load to:0x0202e398 module:main`), which
 *       config/arm9/symbols.txt names `_ZN5Scene11AfterRenderEj`.  So the alias
 *       is a plain C name onto the mangled ARM one, which is the opposite
 *       direction from every other row in this file and is why it is spelled
 *       out.
 *
 * ---- 4. ONE HOSTED arm9 BSS GLOBAL ---------------------------------------
 *
 * data_020a0dbc is arm9 .bss (config/arm9/symbols.txt, `kind:bss`) and nothing
 * in the port hosted it before this seat.  src/func_ov006_02121fa4.c (vtable
 * slot 18) reads it as `volatile short data_020a0dbc[]` at indices 0 and 1 to
 * seed the object's +0x5db0..+0x5db6 stylus block.
 *
 * IT IS SIZED BY ROM SPAN AND NOT BY FIELD WIDTH, which is the rule an
 * undersized host global broke once already: the next arm9 symbol is
 * data_020a0dc0, so the ROM span is FOUR bytes and the two shorts the source
 * reads fill it exactly.  It is declared as eight ints to match every other row
 * in hal/auto_bss.cpp, whose header calls the sizes generous defaults -- the
 * hazard that rule guards against is a host object SHORTER than the ROM span,
 * and this is longer.  It lives inside the DSSTATE bracket so the save state
 * captures it, the same as auto_bss.cpp's rows; it is here rather than there so
 * three sibling lanes editing the same shared file in one run do not collide.
 */

#include "hal/dsstate_seg.h"

DSSTATE_BEGIN
extern "C" {

/* arm9 .bss, ROM span 0x020a0dbc..0x020a0dc0 -- section 4. */
int data_020a0dbc[8];

}
DSSTATE_END

/* ---- 1. the thirteen SharedFilePtr rows ---------------------------------- */
#pragma comment(linker, "/alternatename:?data_ov006_021405d0@@3USharedFilePtr@@A=_data_ov006_021405d0")
#pragma comment(linker, "/alternatename:?data_ov006_021405d8@@3USharedFilePtr@@A=_data_ov006_021405d8")
#pragma comment(linker, "/alternatename:?data_ov006_021405e0@@3USharedFilePtr@@A=_data_ov006_021405e0")
#pragma comment(linker, "/alternatename:?data_ov006_021405e8@@3USharedFilePtr@@A=_data_ov006_021405e8")
#pragma comment(linker, "/alternatename:?data_ov006_021405f0@@3USharedFilePtr@@A=_data_ov006_021405f0")
#pragma comment(linker, "/alternatename:?data_ov006_021405f8@@3USharedFilePtr@@A=_data_ov006_021405f8")
#pragma comment(linker, "/alternatename:?data_ov006_02140600@@3USharedFilePtr@@A=_data_ov006_02140600")
#pragma comment(linker, "/alternatename:?data_ov006_02140608@@3USharedFilePtr@@A=_data_ov006_02140608")
#pragma comment(linker, "/alternatename:?data_ov006_02140610@@3USharedFilePtr@@A=_data_ov006_02140610")
#pragma comment(linker, "/alternatename:?data_ov006_02140618@@3USharedFilePtr@@A=_data_ov006_02140618")
#pragma comment(linker, "/alternatename:?data_ov006_02140620@@3USharedFilePtr@@A=_data_ov006_02140620")
#pragma comment(linker, "/alternatename:?data_ov006_02140628@@3USharedFilePtr@@A=_data_ov006_02140628")
#pragma comment(linker, "/alternatename:?data_ov006_02140638@@3USharedFilePtr@@A=_data_ov006_02140638")

/* ---- 2. the int and short rows ------------------------------------------ */
#pragma comment(linker, "/alternatename:?data_ov006_02140540@@3HA=_data_ov006_02140540")
#pragma comment(linker, "/alternatename:?data_ov006_0214054c@@3HA=_data_ov006_0214054c")
#pragma comment(linker, "/alternatename:?data_ov006_02140560@@3HA=_data_ov006_02140560")
#pragma comment(linker, "/alternatename:?data_ov006_02140564@@3HA=_data_ov006_02140564")
#pragma comment(linker, "/alternatename:?data_ov006_02140568@@3HA=_data_ov006_02140568")
#pragma comment(linker, "/alternatename:?data_ov006_0214056c@@3HA=_data_ov006_0214056c")
#pragma comment(linker, "/alternatename:?data_ov006_0214057c@@3HA=_data_ov006_0214057c")
#pragma comment(linker, "/alternatename:?data_ov006_02140580@@3HA=_data_ov006_02140580")
#pragma comment(linker, "/alternatename:?data_ov006_02140590@@3HA=_data_ov006_02140590")
#pragma comment(linker, "/alternatename:?data_ov006_0214059c@@3HA=_data_ov006_0214059c")
#pragma comment(linker, "/alternatename:?data_ov006_021405a0@@3HA=_data_ov006_021405a0")
#pragma comment(linker, "/alternatename:?data_ov006_021405c0@@3HA=_data_ov006_021405c0")
/* the two that are VTABLES rather than data -- section 2 */
#pragma comment(linker, "/alternatename:?data_ov006_0213b2c4@@3HA=_data_ov006_0213b2c4")
#pragma comment(linker, "/alternatename:?data_ov006_0213b3c4@@3HA=_data_ov006_0213b3c4")
/* the three halfwords the stylus block is seeded from */
#pragma comment(linker, "/alternatename:?data_ov006_0212e044@@3FA=_data_ov006_0212e044")
#pragma comment(linker, "/alternatename:?data_ov006_0212e048@@3FA=_data_ov006_0212e048")
#pragma comment(linker, "/alternatename:?data_ov006_0212e04c@@3FA=_data_ov006_0212e04c")
/* two arm9 globals the middle base's own bodies declare at C++ linkage */
#pragma comment(linker, "/alternatename:?data_0209f5f8@@3EA=_data_0209f5f8")
#pragma comment(linker, "/alternatename:?data_ov006_02141a48@@3PAXA=_data_ov006_02141a48")

/* ---- 2b. the SECOND wave's rows, same shapes -----------------------------
   Two more ints and three `struct G2` objects, all from the element-object
   machinery the first wave pulled in (func_ov006_020cafb4, _020cb030,
   _020ccd78, _020cf758).  data_ov006_02140564 appears TWICE in this file, once
   as ?...@@3HA and once as ?...@@3UG2@@A: two TUs declare the same mount word
   with two different C++ types, so each mangling needs its own row and neither
   is redundant. */
#pragma comment(linker, "/alternatename:?data_ov006_0214058c@@3HA=_data_ov006_0214058c")
#pragma comment(linker, "/alternatename:?data_ov006_021405c8@@3HA=_data_ov006_021405c8")
#pragma comment(linker, "/alternatename:?data_ov006_0213b12c@@3UG2@@A=_data_ov006_0213b12c")
#pragma comment(linker, "/alternatename:?data_ov006_0213b184@@3UG2@@A=_data_ov006_0213b184")
#pragma comment(linker, "/alternatename:?data_ov006_02140564@@3UG2@@A=_data_ov006_02140564")

/* ---- 3. the three bare names, resolved from their literal pools ---------- */
#pragma comment(linker, "/alternatename:__ZTV18dMgTrmpln3DMario_c=_data_ov006_0213b2c4")
#pragma comment(linker, "/alternatename:_func_020beb74=_data_ov004_020beb74")
#pragma comment(linker, "/alternatename:_Scene_AfterRender=__ZN5Scene11AfterRenderEj")
/* src/func_ov006_020d0b2c.cpp spells Model::LoadTextureToVram as a free
   function in a Model namespace, so MSVC mangles the CALL as
   ?LoadTextureToVram@Model@@YAHPADI@Z while the body the port already links
   (src/_ZN5Model17LoadTextureToVramEPcj.cpp, wired by port/slice_gate4b.txt)
   carries the ARM mangling as a plain C name.  config/arm9/symbols.txt names
   the ROM body _ZN5Model17LoadTextureToVramEPcj at 0x02045c10, so the two are
   the same function under two spellings and the row joins them. */
#pragma comment(linker, "/alternatename:?LoadTextureToVram@Model@@YAHPADI@Z=__ZN5Model17LoadTextureToVramEPcj")

/* MSVC emits nothing for a TU that is only pragmas, and an empty object still
   carries the linker directives. This symbol exists so a reader grepping for
   who owns these rows finds a definition rather than only a comment. */
extern "C" const char port_mg_tti_faces_marker[] =
    "dScMgTrampoline_c faces, run mg11 lane TTI";
