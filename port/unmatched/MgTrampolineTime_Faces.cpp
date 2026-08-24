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
 * ---- 2. THE SEVENTEEN int AND short ROWS OF THE FIRST WAVE ---------------
 *
 * Same TU and its neighbours, same shape, different types: FOURTEEN `int`
 * (?...@@3HA) and THREE `short` (?...@@3FA), which is seventeen.  The heading
 * on this section used to say SIXTEEN while the sentence under it said
 * fourteen and three -- a heading and its own body disagreeing by one, which
 * is what a summary written before the list it summarises does.
 *
 * THE WHOLE FILE'S CENSUS, so a reader does not have to add up four waves.
 * Counted off the committed file, 59 /alternatename rows:
 *
 *     13  ?...@@3USharedFilePtr@@A     section 1
 *     25  ?...@@3HA          int       sections 2, 2b, 2c, 2d
 *      4  ?...@@3PAHA        int*      sections 2c, 2d
 *      3  ?...@@3FA          short     section 2
 *      3  ?...@@3UG2@@A      struct G2 section 2b
 *      3  ?...@@3PAXA        void*     sections 2, 2c
 *      1  ?...@@3US8@@A      struct S8 section 2c
 *      1  ?...@@3PAPAXA      void**    section 2c
 *      1  ?...@@3EA          uchar     section 2
 *      5  the bare names and the two C++ spellings, sections 3 and 5
 *
 * data_ov006_0212e044/48/4c are the three halfwords chain link 0
 * (src/func_ov006_02121d64.cpp) and the state setter src/func_ov006_02121f04.c
 * seed the object's +0x5db0..+0x5db6 block from.
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

/* ---- 2d. the FOURTH wave, all the same shape ------------------------- */
#pragma comment(linker, "/alternatename:?data_ov006_0213b1d4@@3PAHA=_data_ov006_0213b1d4")
#pragma comment(linker, "/alternatename:?data_ov006_0213b1dc@@3PAHA=_data_ov006_0213b1dc")
#pragma comment(linker, "/alternatename:?data_ov006_0213b1ec@@3PAHA=_data_ov006_0213b1ec")
#pragma comment(linker, "/alternatename:?data_ov006_02140544@@3HA=_data_ov006_02140544")
#pragma comment(linker, "/alternatename:?data_ov006_02140558@@3HA=_data_ov006_02140558")
#pragma comment(linker, "/alternatename:?data_ov006_02140578@@3HA=_data_ov006_02140578")
#pragma comment(linker, "/alternatename:?data_ov006_021405b0@@3HA=_data_ov006_021405b0")
#pragma comment(linker, "/alternatename:?data_ov006_0214059c@@3PAXA=_data_ov006_0214059c")

/* ---- 2c. the THIRD wave. Same shapes; two of them are a SECOND mangling
   of a word this file already carries (data_ov006_021405c8 as int and as
   int*, data_ov006_0213b22c as void**), which is two TUs disagreeing about
   the type of one mount word rather than a duplicate row. */
#pragma comment(linker, "/alternatename:?data_ov006_021405c8@@3PAHA=_data_ov006_021405c8")
#pragma comment(linker, "/alternatename:?data_ov006_0213b0f0@@3HA=_data_ov006_0213b0f0")
#pragma comment(linker, "/alternatename:?data_ov006_02140574@@3HA=_data_ov006_02140574")
#pragma comment(linker, "/alternatename:?data_ov006_02140598@@3HA=_data_ov006_02140598")
#pragma comment(linker, "/alternatename:?data_ov006_021405a8@@3HA=_data_ov006_021405a8")
#pragma comment(linker, "/alternatename:?data_ov006_021405b4@@3HA=_data_ov006_021405b4")
#pragma comment(linker, "/alternatename:?data_ov006_0213b14c@@3US8@@A=_data_ov006_0213b14c")
#pragma comment(linker, "/alternatename:?data_ov006_0213b22c@@3PAPAXA=_data_ov006_0213b22c")
#pragma comment(linker, "/alternatename:?data_ov006_02141a40@@3PAXA=_data_ov006_02141a40")

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
/* THE FIRST VERSION OF THIS ROW POINTED AT A SYMBOL THAT DOES NOT EXIST and
   is kept as a comment because the failure mode is silent: it named
   __ZN5Model17LoadTextureToVramEPcj, the ITANIUM spelling, on the assumption
   that src/_ZN5Model17LoadTextureToVramEPcj.cpp defines a plain C name.  It
   does not -- the file is a //cpp TU that defines the STATIC MEMBER declared
   in include/Model.h, so MSVC emits ?LoadTextureToVram@Model@@SAIPADI@Z
   (read out of the compiled .obj, not guessed).  TWO /alternatename rows for
   the SAME left side do not both apply: the link kept the first and the
   symbol stayed unresolved through a whole build.  The live row is in
   section 5. */

/* ---- 5. TWO C++ SPELLINGS THAT NEED A SHIM RATHER THAN AN ALIAS -------

   ModelAnim::SetAnim.  src/func_ov006_020cb528.c and _020cb690.cpp call it
   as a METHOD on a shadow class of their own, so MSVC wants
   ?SetAnim@ModelAnim@@QAEXPAXHHI@Z -- __thiscall, receiver in ecx.  The
   definition the port links is
   src/_ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj.c, a plain C free
   function taking the receiver as its FIRST STACK ARGUMENT.

   AN /alternatename BETWEEN THOSE TWO WOULD BE THE FASTCALL-VS-CDECL
   DEFECT, not a fix: the caller would leave the receiver in ecx and the
   callee would read it off the stack.  So this is a real forwarding
   method, which is the shape hal/bob_enemy_shadow_faces.cpp uses for the
   BCA_File* overload of the same function.  Two overloads, two symbols,
   no clash.

   Model::LoadTextureToVram is the opposite case and DOES take an alias.
   src/func_ov006_020d0b2c.cpp spells it as a free function at namespace
   scope (?LoadTextureToVram@Model@@YAHPADI@Z, __cdecl) while
   include/Model.h declares it a STATIC member and
   src/_ZN5Model17LoadTextureToVramEPcj.cpp defines it
   (?LoadTextureToVram@Model@@SAIPADI@Z).  A static member is __cdecl too,
   the parameter lists are identical and int/unsigned return in the same
   register, so the two really are one function under two spellings. */
struct ModelAnim { void SetAnim(void *f, int flags, int speed,
                                unsigned start); };
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        void *self, void *f, int flags, int speed, unsigned start);
void ModelAnim::SetAnim(void *f, int flags, int speed, unsigned start)
{ _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(this, f, flags, speed, start); }

#pragma comment(linker, "/alternatename:?LoadTextureToVram@Model@@YAHPADI@Z=?LoadTextureToVram@Model@@SAIPADI@Z")

/* src/func_ov006_020c7c68.c calls func_ov006_020e6df0, which is not a
   symbol: config/arm9/overlays/ov006/symbols.txt names 0x020e6df0
   Sound_PlayBank1Panned, and delinks.txt gives it its own .text block
   0x020e6df0..0x020e6e3c.  Same bare-name shape as section 3. */
#pragma comment(linker, "/alternatename:_func_ov006_020e6df0=_Sound_PlayBank1Panned")

/* MSVC emits nothing for a TU that is only pragmas, and an empty object still
   carries the linker directives. This symbol exists so a reader grepping for
   who owns these rows finds a definition rather than only a comment. */
extern "C" const char port_mg_tti_faces_marker[] =
    "dScMgTrampoline_c faces, run mg11 lane TTI";
