/* THE MEASURED CLOSURE GAP of the ov092 slice -- run rel0215 wave 2, lane
 * cast-lvl16pair, sub-job 1 of 2.
 *
 * This is not a guess at what the slice would need. It is the FIRST LINK'S OWN
 * UNRESOLVED LIST, taken the way slice_vs.txt section 3 says to take it: wire
 * the slice, link once, and read what the linker asks for. The whole list is
 * ONE symbol, on all three targets:
 *
 *   func_ov092_021313b0.cpp.obj : error LNK2019: unresolved external symbol
 *   "public: int __thiscall PathPtr::GetNode(struct Vector3 &,unsigned int)const"
 *   (?GetNode@PathPtr@@QBEHAAUVector3@@I@Z) referenced in function
 *   _func_ov092_021313b0
 *
 * It is a NAMING mismatch, not a missing body. src/_ZNK7PathPtr7GetNodeER7Vector3j.c
 * is a matched, linked, plain-C body already in this build under the flat
 * Itanium name; src/func_ov092_021313b0.cpp declares its own local shadow
 *
 *     struct PathPtr { int GetNode(Vector3 &v, unsigned int i) const; };
 *
 * and calls it as a member, so MSVC emits a reference to its own decoration and
 * nothing answers. Nothing here changes what code runs and nothing here stands
 * in for a body that was not decompiled.
 *
 * AN /alternatename WOULD BE WRONG AND IS DELIBERATELY NOT USED. An MSVC method
 * is __thiscall and takes its receiver in ECX; the flat Itanium-named definition
 * is cdecl and takes it on the stack. Aliasing one onto the other delivers the
 * receiver in the wrong place -- failure mode 3 in hal/method_faces.cpp's own
 * header, the 2026-08-07 door-open crash. This is a REAL FORWARDER instead, the
 * ov077 Lakitu_ShadowFaces / hal/bob_enemy_shadow_faces.cpp recipe.
 *
 * THE SHADOW IS SPELLED EXACTLY AS THE CALLER SPELLS IT, or the decoration does
 * not match and the gap stays open: `Vector3` is a STRUCT (include/types.h:28),
 * which is what the `AAUVector3@@` in the mangling records, and the index
 * parameter is a plain `unsigned int` (`I`), and the method is `const` (`QBE`).
 * include/PathPtr.h is deliberately NOT included: it declares the real class,
 * and having both in one TU is a redefinition -- the ov077 TextureSequence
 * lesson.
 *
 * THE RETURN VALUE IS 0 AND THAT IS SAFE HERE, checked rather than assumed --
 * this is the exact place the VS lane went wrong (a face returning 0 where the
 * real body returns a pointer the caller used). Two facts settle it:
 *   * the ROM body returns NOTHING. src/_ZNK7PathPtr7GetNodeER7Vector3j.c is
 *     `void _ZNK7PathPtr7GetNodeER7Vector3j(void *thiz, int *vec, unsigned idx)`
 *     and writes its three results THROUGH the out-pointer, so there is no
 *     value to lose;
 *   * the one caller discards it. func_ov092_021313b0's call is a statement:
 *     `((PathPtr*)(c+0x58c))->GetNode(*(Vector3*)(c+0x580), ...);` -- the
 *     result is never read, and the `int` in the shadow's declaration is the
 *     recovery's own placeholder, not something the ROM produces.
 * The out-parameter, which IS the real output, is forwarded unchanged.
 *
 * WHY THIS FILE IS LANE-OWNED rather than an append to hal/method_faces.cpp or
 * hal/cxxname_bridge.cpp, where it would naturally live: no lane owns those
 * files this wave, and a shared append to either is a merge conflict with every
 * other wave-2 lane for no functional gain. The ov045/ov077 lane-ownership
 * precedent. Whoever next owns those files should move this one forwarder
 * there; it is written so the move is a cut and paste.
 */
struct Vector3;

struct PathPtr {
    int GetNode(Vector3 &v, unsigned int i) const;
};

extern "C" void _ZNK7PathPtr7GetNodeER7Vector3j(void *thiz, void *vec,
                                                unsigned int idx);

/* PORT_HOST_ABI: __thiscall receiver in ECX forwarded to the cdecl flat name. */
int PathPtr::GetNode(Vector3 &v, unsigned int i) const
{
    _ZNK7PathPtr7GetNodeER7Vector3j(const_cast<PathPtr *>(this), &v, i);
    return 0;
}
