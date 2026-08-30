/* THE MEASURED CLOSURE GAP of the ov077 slice -- run rel0215 lane cast-ov077.
 *
 * These eight symbols are not a guess at what the slice would need. They are
 * the FIRST LINK'S OWN UNRESOLVED LIST, taken the way slice_vs.txt section 3
 * says to take it: wire the slice, link once, and read what the linker asks
 * for. Every one of them is a NAMING mismatch between what a matched ov077 TU
 * spells and what the rest of the port already defines under another spelling;
 * not one of them is a missing body. Nothing here changes what code runs, and
 * nothing here is a stand-in for a body that was not decompiled.
 *
 * ---- 1. FIVE MOUNT SYMBOLS SPELLED AS C++ TYPES (data, no ABI) ------------
 * Three matched TUs declare mounted ov077 data with a C++ type and no
 * extern "C", so MSVC decorates the reference while ovdata.py's mount defines
 * the plain C name:
 *   src/_ZN5Spiny13InitResourcesEv.cpp   `extern SharedFilePtr data_ov077_...`
 *       -> ?data_ov077_02127b38@@3USharedFilePtr@@A  (file 0x42a)
 *       -> ?data_ov077_02127b48@@3USharedFilePtr@@A  (file 0x429)
 *       -> ?data_ov077_02127c14@@3USharedFilePtr@@A  (file 0x42b)
 *   src/func_ov077_021244d4.cpp          `extern void *data_ov077_...[]`
 *       -> ?data_ov077_02127b30@@3PAPAXA
 *       -> ?data_ov077_02127b40@@3PAPAXA
 * These are DATA. There is no calling convention to get wrong, both names
 * denote the same storage, and an /alternatename is exactly right -- the
 * cxxname_bridge.cpp:260 shape (`?data_ov098_0213c380@@3PADA=_data_ov098_
 * 0213c380`). Each LHS is an MSVC decoration nothing in the link defines, so
 * none of these five can be silently defeated the way an alias onto an
 * already-defined LHS is; port/tools/alternatename_guard.py re-checks that
 * against walk_window.map after every link, which is why none of them belongs
 * in port/tools/alternatename_baseline.txt (that file is the allowlist of
 * aliases whose LHS IS deliberately defined).
 *
 * ---- 2. TWO METHOD SPELLINGS (code -- and here the ABI DOES matter) -------
 * An /alternatename would be WRONG for both of these and is deliberately not
 * used. An MSVC method is __thiscall and takes its receiver in ECX; the flat
 * Itanium-named definitions in this port are cdecl and take it on the stack.
 * Aliasing one onto the other delivers the receiver in the wrong place --
 * failure mode 3 in hal/method_faces.cpp's own header, the 2026-08-07
 * door-open crash. Both are REAL FORWARDERS instead.
 *
 *   BgCh::StopDetectingWater. src/func_ov077_02123c6c.cpp and
 *   src/func_ov077_02124c28.cpp call the flat _ZN4BgCh18StopDetectingWaterEv,
 *   but the matched body src/_ZN4BgCh18StopDetectingWaterEv.cpp (already in
 *   the build via port/slice_gate10.txt:854) is a real C++ method, so only
 *   ?StopDetectingWater@BgCh@@QAEXXZ exists. This is the exact twin of
 *   _ZN4BgCh19StartDetectingWaterEv at hal/player_bridges.cpp:915, written the
 *   same way. The decompiled body is what runs; this only carries the ROM's C
 *   name to it (a FORWARDS face in port/faces_adjudicated.txt's terms).
 *
 *   TextureSequence::SetFile. src/func_ov077_021244d4.cpp declares its own
 *   local shadow `struct TextureSequence { void SetFile(BTP_File &, int,
 *   Fix12, unsigned); }` and calls it as a member, so MSVC emits a reference
 *   to ?SetFile@TextureSequence@@QAEXAAUBTP_File@@HHI@Z. The definition in
 *   this port carries the Itanium C name
 *   _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj. The identical shadow
 *   is redeclared below and its method defined to forward -- the
 *   hal/bob_enemy_shadow_faces.cpp recipe, which does the same for
 *   ModelAnim::SetAnim, Camera::SetPos and ApproachLinear. The shadow must be
 *   spelled EXACTLY as the caller spells it (Fix12 is a plain int there, so
 *   the third parameter is `int`, giving ...@@QAEXAAUBTP_File@@HHI@Z) or the
 *   decoration does not match and the gap stays open.
 *   include/TextureSequence.h is deliberately NOT included here: it declares
 *   the real class, and having both in one TU is a redefinition.
 *
 * WHY THIS FILE IS LANE-OWNED RATHER THAN AN APPEND TO hal/method_faces.cpp,
 * hal/player_bridges.cpp or hal/cxxname_bridge.cpp, which is where its two
 * halves would naturally live: no lane owns those three files this wave, and
 * a shared append to any of them is a merge conflict with every other wave-1
 * lane for no functional gain. The ov045 lane-ownership precedent. Whoever
 * next owns those files should move these seven declarations there; they are
 * written so that the move is a cut and paste.
 */
#include "BgCh.h"

/* ---- 1. the five data spellings ---------------------------------------- */
#pragma comment(linker, "/alternatename:?data_ov077_02127b30@@3PAPAXA=_data_ov077_02127b30")
#pragma comment(linker, "/alternatename:?data_ov077_02127b38@@3USharedFilePtr@@A=_data_ov077_02127b38")
#pragma comment(linker, "/alternatename:?data_ov077_02127b40@@3PAPAXA=_data_ov077_02127b40")
#pragma comment(linker, "/alternatename:?data_ov077_02127b48@@3USharedFilePtr@@A=_data_ov077_02127b48")
#pragma comment(linker, "/alternatename:?data_ov077_02127c14@@3USharedFilePtr@@A=_data_ov077_02127c14")

/* ---- 2a. BgCh::StopDetectingWater, the C name onto the matched method ---- */
/* PORT_HOST_ABI: C-linkage face onto a matched C++ method; the twin of
 * _ZN4BgCh19StartDetectingWaterEv in hal/player_bridges.cpp. */
extern "C" void _ZN4BgCh18StopDetectingWaterEv(void *self)
{ ((BgCh *)self)->BgCh::StopDetectingWater(); }

/* ---- 2b. TextureSequence::SetFile, the caller's shadow onto the C name --- */
struct BTP_File;
struct TextureSequence {
    void SetFile(BTP_File &animFile, int flags, int speed, unsigned int start);
};
extern "C" void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
        void *self, void *btp, int flags, int speed, unsigned int start);

/* PORT_HOST_ABI: shadow-struct method onto the Itanium C name, the
 * ModelAnim::SetAnim shape in hal/bob_enemy_shadow_faces.cpp. */
void TextureSequence::SetFile(BTP_File &animFile, int flags, int speed,
                              unsigned int start)
{
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(this, &animFile, flags,
                                                       speed, start);
}
