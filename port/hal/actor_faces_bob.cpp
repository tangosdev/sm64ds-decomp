// Gate 33: the cross-language faces and data aliases the twelve classes in
// hal/actor_classes_bob_world.cpp need, and nothing else.
//
// THIS FILE INCLUDES NOTHING, for the reason hal/actor_class_faces.cpp gives
// at length: a decorated name is its own name, its class's name, its calling
// convention and its parameter types, and nothing about layout. Re-declaring a
// class here with only the one method reproduces exactly the symbol the caller
// emitted; pulling in the real header would redefine those names and could
// silently change a parameter type into a symbol nobody is asking for.
//
// Three kinds of entry live here.
//
//   1. DATA ALIASES. The class TUs were recovered as C++ and spell their
//      globals with C++ linkage, so a reference comes out as
//      ?data_ov002_0210de00@@3DA while the definition the ov002 mount emits is
//      the plain C name. Data has no calling convention, so an alias is exact.
//      Several of the same global appear twice with different declared TYPES
//      (@@3DA is `char`, @@3PAXA is `void *`) because two TUs declared it
//      differently; both alias onto the one object.
//
//   2. PLACEHOLDER-NAME FACES. Some TUs invented a plain name for a function
//      whose real symbol is Itanium-mangled -- ModelBase_SetFile,
//      Platform_UpdateClsnPosAndRot and the rest. Real forwarding definitions
//      rather than aliases so the names stay readable in a map file.
//
//   3. METHOD FACES. A caller asking for an MSVC __thiscall member against a
//      cdecl definition, or the reverse. Never an alias: `this` rides in ecx
//      on one side and on the stack on the other.
#include <cstddef>

extern "C" {
void *_ZN5Model8LoadFileER13SharedFilePtr(void *ptr);
void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(void *ptr);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *file, int a, int b);
void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, const void *mat, int scale, short angY, void *clps);
void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);
void _ZN8Platform21UpdateModelPosAndRotYEv(void *self);
int _ZN11ShadowModel12InitCylinderEv(void *self);
}

// ---- 2. the placeholder names ---------------------------------------------
//
// FortressTower's InitResources (the warp pipe's) reaches six functions
// through names its own TU invented. All six are plain cdecl on both sides,
// so these are ordinary forwards.
extern "C" void *Model_LoadFile(void *ptr)
{ return _ZN5Model8LoadFileER13SharedFilePtr(ptr); }
extern "C" void *MeshCollider_LoadFile(void *ptr)
{ return _ZN12MeshCollider8LoadFileER13SharedFilePtr(ptr); }
extern "C" int ModelBase_SetFile(void *self, void *file, int a, int b)
{ return _ZN9ModelBase7SetFileEP8BMD_Fileii(self, file, a, b); }
extern "C" void MovingMeshCollider_SetFile(void *self, void *kcl,
                                           const void *mat, int scale,
                                           short angY, void *clps)
{
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        self, kcl, mat, scale, angY, clps);
}
extern "C" void Platform_UpdateClsnPosAndRot(void *self)
{ _ZN8Platform19UpdateClsnPosAndRotEv(self); }
extern "C" void Platform_UpdateModelPosAndRotY(void *self)
{ _ZN8Platform21UpdateModelPosAndRotYEv(self); }

// ---- 3. the method faces ---------------------------------------------------
//
// WaterBomb::InitResources reaches three of these as members of its own shadow
// declarations. Model::LoadFile is a STATIC, so it is cdecl on both sides and
// the alias is exact; the other two are instance methods and get bodies.
#pragma comment(linker, "/alternatename:?LoadFile@Model@@SAPAUBMD_File@@AAUSharedFilePtr@@@Z=__ZN5Model8LoadFileER13SharedFilePtr")

struct BMD_File;
struct SharedFilePtr;

/* ?SetFile@ModelBase@@QAEHPAUBMD_File@@HH@Z -- an INT-returning SetFile, while
   include/ModelBase.h declares it void and mangles to @@QAEX...@Z. The ROM's
   ModelBase::SetFile tail-calls DoSetFile and rides its r0 out, so the int is
   the truthful shape and the header is the lossy one; the definition cannot be
   spelled twice, so this is the shadow-plus-alias form rather than a bare
   alias onto the void version, which would leave eax undefined. */
struct ModelBaseFace { int SetFile(BMD_File *file, int a, int b); };
int ModelBaseFace::SetFile(BMD_File *file, int a, int b)
{ return _ZN9ModelBase7SetFileEP8BMD_Fileii(this, file, a, b); }
#pragma comment(linker, "/alternatename:?SetFile@ModelBase@@QAEHPAUBMD_File@@HH@Z=?SetFile@ModelBaseFace@@QAEHPAUBMD_File@@HH@Z")

/* ?InitCylinder@ShadowModel@@QAEHXZ -- the cylinder shadow, the sibling of the
   InitCuboid face hal/actor_class_faces.cpp already carries. */
struct ShadowModelFace { int InitCylinder(); };
int ShadowModelFace::InitCylinder()
{ return _ZN11ShadowModel12InitCylinderEv(this); }
#pragma comment(linker, "/alternatename:?InitCylinder@ShadowModel@@QAEHXZ=?InitCylinder@ShadowModelFace@@QAEHXZ")

// ---- C++-LINKAGE references onto C definitions ------------------------------
//
// Free functions, cdecl on both sides, so these are aliases. The three
// Itanium-spelled ones are TUs that put the mangled name inside a C++ block
// instead of an extern "C" one, which makes MSVC mangle the mangling.
#pragma comment(linker, "/alternatename:?Vec3_Equal@@YAHPAX0@Z=_Vec3_Equal")
#pragma comment(linker, "/alternatename:?_ZN5Actor13ClosestPlayerEv@@YAHPAX@Z=__ZN5Actor13ClosestPlayerEv")
#pragma comment(linker, "/alternatename:?_ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_@@YAXPAX0HH0H@Z=__ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_")
#pragma comment(linker, "/alternatename:?func_ov001_020ab228@@YAXPAX0HHH@Z=_func_ov001_020ab228")
#pragma comment(linker, "/alternatename:?func_ov002_020b7f2c@@YAXPAX0@Z=_func_ov002_020b7f2c")
#pragma comment(linker, "/alternatename:?func_ov002_020b7f7c@@YAXPAX@Z=_func_ov002_020b7f7c")

// ---- 1. the data aliases ---------------------------------------------------
#pragma comment(linker, "/alternatename:?data_02092138@@3HA=_data_02092138")
#pragma comment(linker, "/alternatename:?data_ov002_021098e8@@3DA=_data_ov002_021098e8")
#pragma comment(linker, "/alternatename:?data_ov002_021098ec@@3DA=_data_ov002_021098ec")
#pragma comment(linker, "/alternatename:?data_ov002_0210d7f4@@3UCLPS_Block@@A=_data_ov002_0210d7f4")
#pragma comment(linker, "/alternatename:?data_ov002_0210d9a8@@3USharedFilePtr@@A=_data_ov002_0210d9a8")
#pragma comment(linker, "/alternatename:?data_ov002_0210d9e8@@3USharedFilePtr@@A=_data_ov002_0210d9e8")
#pragma comment(linker, "/alternatename:?data_ov002_0210da08@@3USharedFilePtr@@A=_data_ov002_0210da08")
#pragma comment(linker, "/alternatename:?data_ov002_0210da28@@3USharedFilePtr@@A=_data_ov002_0210da28")
#pragma comment(linker, "/alternatename:?data_ov002_0210da38@@3USharedFilePtr@@A=_data_ov002_0210da38")
#pragma comment(linker, "/alternatename:?data_ov002_0210de00@@3DA=_data_ov002_0210de00")
#pragma comment(linker, "/alternatename:?data_ov002_0210de00@@3PAXA=_data_ov002_0210de00")
#pragma comment(linker, "/alternatename:?data_ov002_0210de08@@3DA=_data_ov002_0210de08")
#pragma comment(linker, "/alternatename:?data_ov002_0210de08@@3PAXA=_data_ov002_0210de08")
#pragma comment(linker, "/alternatename:?data_ov002_0210de10@@3DA=_data_ov002_0210de10")
#pragma comment(linker, "/alternatename:?data_ov002_0210de10@@3PAXA=_data_ov002_0210de10")
#pragma comment(linker, "/alternatename:?data_ov002_0210de18@@3DA=_data_ov002_0210de18")
#pragma comment(linker, "/alternatename:?data_ov002_0210de18@@3PAXA=_data_ov002_0210de18")
#pragma comment(linker, "/alternatename:?data_ov002_0210de20@@3DA=_data_ov002_0210de20")
#pragma comment(linker, "/alternatename:?data_ov002_0210de20@@3PAXA=_data_ov002_0210de20")
#pragma comment(linker, "/alternatename:?data_ov002_0210de28@@3DA=_data_ov002_0210de28")
#pragma comment(linker, "/alternatename:?data_ov002_0210de28@@3PAXA=_data_ov002_0210de28")
#pragma comment(linker, "/alternatename:?data_ov002_0210de30@@3DA=_data_ov002_0210de30")
#pragma comment(linker, "/alternatename:?data_ov002_0210de30@@3PAXA=_data_ov002_0210de30")
#pragma comment(linker, "/alternatename:?data_ov002_0210de38@@3DA=_data_ov002_0210de38")
#pragma comment(linker, "/alternatename:?data_ov002_0210de38@@3PAXA=_data_ov002_0210de38")
#pragma comment(linker, "/alternatename:?data_ov002_0210de40@@3DA=_data_ov002_0210de40")
#pragma comment(linker, "/alternatename:?data_ov002_0210de40@@3PAXA=_data_ov002_0210de40")
#pragma comment(linker, "/alternatename:?data_ov002_0210de48@@3DA=_data_ov002_0210de48")
#pragma comment(linker, "/alternatename:?data_ov002_0210de48@@3PAXA=_data_ov002_0210de48")
#pragma comment(linker, "/alternatename:?data_ov002_0210de50@@3DA=_data_ov002_0210de50")
#pragma comment(linker, "/alternatename:?data_ov002_0210de50@@3PAXA=_data_ov002_0210de50")
#pragma comment(linker, "/alternatename:?data_ov002_0210de58@@3DA=_data_ov002_0210de58")
#pragma comment(linker, "/alternatename:?data_ov002_0210de58@@3PAXA=_data_ov002_0210de58")
#pragma comment(linker, "/alternatename:?data_ov002_0210de60@@3DA=_data_ov002_0210de60")
#pragma comment(linker, "/alternatename:?data_ov002_0210de60@@3PAXA=_data_ov002_0210de60")
#pragma comment(linker, "/alternatename:?data_ov002_0210df54@@3DA=_data_ov002_0210df54")
#pragma comment(linker, "/alternatename:?data_ov002_0210e124@@3USharedFilePtr@@A=_data_ov002_0210e124")
#pragma comment(linker, "/alternatename:?data_ov002_0210e12c@@3USharedFilePtr@@A=_data_ov002_0210e12c")
#pragma comment(linker, "/alternatename:?data_ov002_0211092c@@3DA=_data_ov002_0211092c")
#pragma comment(linker, "/alternatename:?data_ov098_0213c91c@@3USharedFilePtr@@A=_data_ov098_0213c91c")
#pragma comment(linker, "/alternatename:?data_ov100_02148668@@3DA=_data_ov100_02148668")
#pragma comment(linker, "/alternatename:?data_ov100_0214867c@@3PAUVtEntry@@A=_data_ov100_0214867c")
#pragma comment(linker, "/alternatename:?data_ov102_0214e188@@3PADA=_data_ov102_0214e188")
#pragma comment(linker, "/alternatename:?data_ov102_0214e188@@3PAPAXA=_data_ov102_0214e188")
#pragma comment(linker, "/alternatename:?data_ov102_0214e18c@@3PADA=_data_ov102_0214e18c")
#pragma comment(linker, "/alternatename:?data_ov102_0214e18c@@3PAPAXA=_data_ov102_0214e18c")

/* The ROM's own RTTI name for the arrow signs' table; ArrowSignRight's D0
   spells it that way. */
#pragma comment(linker, "/alternatename:__ZTV15daObjYajirusi_c=__ZTV14ArrowSignRight")

/* --- linkloop round 4 -------------------------------------------------------
   Two more static members and two more data spellings. Actor::Spawn and
   Actor::FindWithActorID are STATICS, cdecl on both sides, so the aliases are
   exact -- there is no `this` to lose. */
#pragma comment(linker, "/alternatename:?Spawn@Actor@@SAPAU1@IIABUVector3@@PBUVector3_16@@HH@Z=__ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii")
#pragma comment(linker, "/alternatename:?FindWithActorID@Actor@@SAPAV1@IPAV1@@Z=__ZN5Actor15FindWithActorIDEjPS_")
#pragma comment(linker, "/alternatename:?data_ov002_0210e00c@@3PAUEntry@@A=_data_ov002_0210e00c")
#pragma comment(linker, "/alternatename:?data_ov098_0213bf90@@3PAGA=_data_ov098_0213bf90")

/* Model::Render by its Itanium C name, from the CAP's own Render. The
   definition is a real MSVC __thiscall method (src/_ZN5Model6RenderEPK7Vector3
   .cpp), so this is a face, not an alias. */
struct Vector3;
extern "C" void _ZN5Model6RenderEPK7Vector3(void *self, const void *scale);
#ifdef _WIN32
struct ModelRenderFace { void Render(const Vector3 *scale); };
#pragma comment(linker, "/alternatename:?Render@ModelRenderFace@@QAEXPBUVector3@@@Z=?Render@Model@@UAEXPBUVector3@@@Z")
extern "C" void _ZN5Model6RenderEPK7Vector3(void *self, const void *scale)
{ ((ModelRenderFace *)self)->Render((const Vector3 *)scale); }
#endif /* _WIN32: on GCC the ModelRenderFace::Render alias name IS the Itanium
   mangling of Model::Render, so this face self-recurses. Linux binds callers
   straight to the real src/_ZN5Model6RenderEPK7Vector3.cpp (in the build since
   gate 4b/33). */

/* Player::CanWarp -- the warp's own gate. src/_ZN6Player7CanWarpEv.cpp defines
   it against include/Player.h, where it returns int; func_ov002_020ec410
   declares it on its own shadow Player returning bool, and bool and int are
   different types in a decorated name (_N against H). Same class, same
   __thiscall, same absence of arguments, so the alias is exact -- and the ROM
   returns 0 or 1 in r0 either way. */
#pragma comment(linker, "/alternatename:?CanWarp@Player@@QAE_NXZ=?CanWarp@Player@@QAEHXZ")

/* CylinderClsn::Process is a STATIC member (it walks a global list and takes
   no `this`), so it is cdecl on both sides and the alias is exact. The
   registry's call site spells it by its Itanium name. */
#pragma comment(linker, "/alternatename:__ZN12CylinderClsn7ProcessEv=?Process@CylinderClsn@@SAXXZ")

/* src/func_02014f5c.c calls ov002 0x020caf98 by an UNPREFIXED name: its own TU
   had no way to know the address lands in an overlay, so it spelled it
   func_020caf98 while the definition carries the ov002 prefix. One function,
   two spellings, both cdecl. */
#pragma comment(linker, "/alternatename:_func_020caf98=_func_ov002_020caf98")

/* CylinderClsn::Process's own closure ends in ov002 0x020caf98, the Player's
   "you were hit" path, which reaches four Player::State objects and
   Player::ChangeState with C++ linkage. The four States are mounted ov002 data
   and the alias is exact; ChangeState is the port's host copy in
   port/unmatched/Player_ChangeState.cpp, which spells the argument as a
   pointer where this caller spells it as a reference -- same address either
   way, and both are __thiscall on the same class. */
#pragma comment(linker, "/alternatename:?data_ov002_0211013c@@3UState@Player@@A=_data_ov002_0211013c")
#pragma comment(linker, "/alternatename:?data_ov002_021101b4@@3UState@Player@@A=_data_ov002_021101b4")
#pragma comment(linker, "/alternatename:?data_ov002_0211031c@@3UState@Player@@A=_data_ov002_0211031c")
#pragma comment(linker, "/alternatename:?data_ov002_021106dc@@3UState@Player@@A=_data_ov002_021106dc")

/* ?ChangeState@Player@@QAEHAAUState@1@@Z -- the caller declares it a member of
   its own shadow Player; the definition is the host copy's C-named
   _ZN6Player11ChangeStateERNS_5StateE. __thiscall against cdecl, so a face. */
extern "C" int _ZN6Player11ChangeStateERNS_5StateE(void *self, void *state);
struct PlayerChangeStateFace {
    struct State;
    int ChangeState(State &st);
};
int PlayerChangeStateFace::ChangeState(State &st)
{ return _ZN6Player11ChangeStateERNS_5StateE(this, &st); }
#pragma comment(linker, "/alternatename:?ChangeState@Player@@QAEHAAUState@1@@Z=?ChangeState@PlayerChangeStateFace@@QAEHAAUState@1@@Z")
