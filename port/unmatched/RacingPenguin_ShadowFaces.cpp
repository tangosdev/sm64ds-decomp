/* The cross-language faces RACING_PENGUIN's closure needs, and nothing else.
 * Run rel0215 wave 3, lane w3-e.
 *
 * THIS IS THE FIRST LINK'S OWN UNRESOLVED LIST, taken the way slice_vs.txt
 * section 3 says to take it: wire the slice, link once, read what the linker
 * asks for. Ten symbols on all three targets, in three kinds.
 *
 * THIS FILE INCLUDES NOTHING but <cstddef>, for the reason
 * hal/actor_faces_bob.cpp gives at length: a decorated name is its own name,
 * its class's name, its calling convention and its parameter types, and
 * nothing about layout. Re-declaring a class here with only the one method
 * reproduces exactly the symbol the caller emitted; pulling in the real header
 * would redefine those names and could silently change a parameter type.
 *
 *   1. PLACEHOLDER-NAME FACES (6). src/_ZN13RacingPenguin13InitResourcesEv.cpp
 *      reaches six functions through flat names its own TU invented --
 *      Actor_TrackStar and friends, all declared in include/decl_common.h and
 *      none of them a real ROM symbol. Five forward to matched plain-C bodies
 *      already in the build; the sixth (TextureSequence_LoadFile) forwards to
 *      a matched C++ METHOD and so goes through a shadow.
 *
 *   2. C++-LINKAGE REFERENCES ONTO C DEFINITIONS (3).
 *      src/_ZN13RacingPenguin8BehaviorEv.cpp declares three Itanium-named
 *      functions OUTSIDE its extern "C" block, so MSVC mangles the mangling.
 *      All three already have plain-C definitions in this build
 *      (hal/method_faces.cpp and hal/player_bridges.cpp), both sides are cdecl,
 *      and data-free free functions alias exactly.
 *
 *   3. A METHOD FACE (1). src/func_ov019_0211127c.cpp declares its own local
 *      `struct PathPtr { void GetNode(Vector3 &, unsigned int) const; }` and
 *      calls it as a member, so MSVC emits ?GetNode@PathPtr@@QBEXAAUVector3@@I@Z
 *      -- note the X: this shadow returns VOID, where the one
 *      unmatched/ToxBox_ShadowFaces.cpp already carries returns int (@@QBEH...)
 *      and is a different symbol. AN /alternatename WOULD BE WRONG: an MSVC
 *      method is __thiscall and takes its receiver in ECX while the flat
 *      Itanium body is cdecl and takes it on the stack, so this is a REAL
 *      FORWARDER, the ToxBox / Lakitu_ShadowFaces recipe. The ROM body returns
 *      nothing (src/_ZNK7PathPtr7GetNodeER7Vector3j.c is `void`) and writes its
 *      result through the out-pointer, so the void shadow is the truthful one
 *      and there is no value to lose.
 *
 * Nothing here changes what code runs and nothing here stands in for a body
 * that was not decompiled.
 */
#include <cstddef>

struct Vector3;

extern "C" {
/* the matched plain-C bodies these forward to, all already linked */
int _ZN5Actor9TrackStarEjj(void *self, unsigned int a, unsigned int b);
void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(
    void *self, void *actor, int radius, int height, unsigned flags,
    unsigned vulnFlags);
void _ZN7PathPtr6FromIDEj(void *self, unsigned int id);
void _ZNK7PathPtr7GetNodeER7Vector3j(void *thiz, void *vec, unsigned int idx);
void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(
    void *self, void *actor, int a2, int a3, void *sp0, int sp1);
}

// ---- 1. the placeholder names ----------------------------------------------
//
// Signatures are decl_common.h's, so the callers' own declarations match:
//   1834 unsigned char Actor_TrackStar(void*, unsigned, unsigned)
//   1932 void MovingCylinderClsn_Init(void*, void*, int, int, unsigned, unsigned)
//   1937 void PathPtr_FromID(void*, unsigned)
//   1938 void PathPtr_GetNode(void*, void*, unsigned)
//   1971 void WithMeshClsn_Init(void*, void*, int, int, void*, int)
//   2973 void *TextureSequence_LoadFile(void*)
extern "C" unsigned char Actor_TrackStar(void *self, unsigned int a,
                                         unsigned int b)
{ return (unsigned char)_ZN5Actor9TrackStarEjj(self, a, b); }

extern "C" void MovingCylinderClsn_Init(void *self, void *actor, int radius,
                                        int height, unsigned int flags,
                                        unsigned int vulnFlags)
{
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(self, actor, radius,
                                                       height, flags,
                                                       vulnFlags);
}

extern "C" void PathPtr_FromID(void *self, unsigned int id)
{ _ZN7PathPtr6FromIDEj(self, id); }

extern "C" void PathPtr_GetNode(void *self, void *vec, unsigned int idx)
{ _ZNK7PathPtr7GetNodeER7Vector3j(self, vec, idx); }

extern "C" void WithMeshClsn_Init(void *self, void *actor, int a2, int a3,
                                  void *sp0, int sp1)
{
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(self, actor, a2,
                                                               a3, sp0, sp1);
}

/* TextureSequence::LoadFile is a STATIC member -- include/TextureSequence.h
   declares it so and hal/player_bridges.cpp:995 calls it with no receiver --
   so it is cdecl on both sides and this is an ordinary forward onto the flat
   Itanium name that file already defines, exactly like the five above. (The
   Model::LoadFile row in hal/actor_faces_bob.cpp is the same static case; the
   instance-method case, which needs a shadow instead, is item 3 below.) */
extern "C" void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(void *fp);
extern "C" void *TextureSequence_LoadFile(void *ptr)
{ return _ZN15TextureSequence8LoadFileER13SharedFilePtr(ptr); }

// ---- 2. C++-LINKAGE references onto C definitions ---------------------------
//
// Free functions, cdecl on both sides, so these are aliases. All three targets
// are defined in this build as plain C names.
#pragma comment(linker, "/alternatename:?_ZN9Animation7AdvanceEv@@YAXPAD@Z=__ZN9Animation7AdvanceEv")
#pragma comment(linker, "/alternatename:?_ZN12CylinderClsn5ClearEv@@YAXPAD@Z=__ZN12CylinderClsn5ClearEv")
#pragma comment(linker, "/alternatename:?_ZN12CylinderClsn6UpdateEv@@YAXPAD@Z=__ZN12CylinderClsn6UpdateEv")

// ---- 3. the method face ------------------------------------------------------
//
// The shadow is spelled EXACTLY as src/func_ov019_0211127c.cpp spells it, or
// the decoration does not match and the gap stays open: `Vector3` is a struct,
// the index is a plain `unsigned int`, the method is `const`, and it returns
// VOID.
struct PathPtr { void GetNode(Vector3 &v, unsigned int i) const; };

/* PORT_HOST_ABI: __thiscall receiver in ECX forwarded to the cdecl flat name. */
void PathPtr::GetNode(Vector3 &v, unsigned int i) const
{
    _ZNK7PathPtr7GetNodeER7Vector3j((void *)this, (void *)&v, i);
}
