// Gate 16: METHOD faces for the actor classes' own shadow declarations.
//
// The class TUs the registry compiles do not include the port's headers. Each
// declares, inline, the minimum shape it needs of whatever it calls -- and
// when that shape is a MEMBER FUNCTION, MSVC emits a __thiscall reference to a
// name that no definition in the tree carries. `this` rides in ecx there and
// on the stack in the C definition, so a linker alias would hand the body a
// stranger's pointer; each one needs a real forwarding method instead.
//
// THE SHADOW DECLARATIONS BELOW ARE THE CALLERS', NOT THE HEADERS'. A method's
// decorated name is its own name, its class's name, its calling convention and
// its parameter types -- nothing about layout -- so re-declaring the class here
// with only the one method reproduces exactly the symbol the caller emitted.
// This file therefore includes NOTHING: pulling in Actor.h or WithMeshClsn.h
// would redefine these names and, worse, could silently change a parameter
// type (Fix12i vs int) and mangle to a symbol nobody is asking for.
//
// Verified against the link log: every decorated name below appeared as an
// unresolved external, and none of them resolves after this file except by
// this file.

struct Actor;
struct dCc_c;
struct Vector3_16;
struct KCL_File;
struct Matrix4x3;
struct CLPS_Block;
struct SharedFilePtr;

extern "C" {
void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *clsn);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *actor,
                                                        int r, int h,
                                                        unsigned e,
                                                        unsigned f);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, const void *mat, int scale, short angY, void *clps);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *self, void *actor, int r, int h, void *v, int t);
void _ZN8dActor_c28UpdatePosWithHorzSpeedAndAngEv(void *self);
short _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(void *self, int a, int b, short c);
void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void *self, void *clsn,
                                              unsigned flags);
int _ZNK10dBgCh_Actr8IsOnWallEv(void *self);
int _ZNK10dBgCh_Actr13JustHitGroundEv(void *self);
void *_ZN5Model8LoadFileER13SharedFilePtr(void *ptr);
void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *ptr);
/* gate 42: the two methods PeachPainting's Behavior reaches by shadow name.
   DistToCPlayer is C-named in src (slice_gate16); the ROM's ApplyOpacity is a
   one-argument body (src/_ZN9ModelBase12ApplyOpacityEj.cpp), but the painting's
   call passes the ROM's r2=1 as a second argument the body ignores, so the face
   forwards only the opacity, the gate-16 reading of the same method. */
int _ZN8dActor_c13DistToCPlayerEv(void *self);
void _ZN9ModelBase12ApplyOpacityEj(void *self, unsigned a);
}

/* Actor: three methods the 1-up's type bodies and the sign's thrown state
   reach.
     ?UpdatePos@dActor_c@@QAEXPAUCylinderClsn@@@Z
     ?UpdatePosWithHorzSpeedAndAng@dActor_c@@QAEXXZ
     ?ReflectAngle@dActor_c@@QAEFHHF@Z  (the C definition spells the two Fix12i
       arguments as the template type, which mangles differently -- same
       function, and the reloc in the caller is what settles it) */
struct Actor {
    void UpdatePos(dCc_c *clsn);
    void UpdatePosWithHorzSpeedAndAng();
    short ReflectAngle(int a, int b, short c);
    int DistToCPlayer();          /* gate 42: PeachPainting::Behavior */
    /* DECLARED AND NOT DEFINED, unlike the four above, and the difference is
       the whole point. Those four forward the other way, MSVC method onto a C
       name. This one is resolved BY THE MATCHED TU:
       src/_ZN8dActor_c17TrackInDeathTableEv.cpp compiles
       ?TrackInDeathTable@dActor_c@@QAEXXZ, so a definition here would be a
       duplicate symbol rather than a bridge. The empty-bodied members differ
       in layout from that TU's Actor and it does not matter -- MSVC mangles a
       method by name and signature, and a non-virtual call on a single class
       passes `this` unadjusted. */
    void TrackInDeathTable();
};
void Actor::UpdatePos(dCc_c *clsn)
{ _ZN8dActor_c9UpdatePosEP5dCc_c(this, clsn); }
void Actor::UpdatePosWithHorzSpeedAndAng()
{ _ZN8dActor_c28UpdatePosWithHorzSpeedAndAngEv(this); }
short Actor::ReflectAngle(int a, int b, short c)
{ return _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(this, a, b, c); }
int Actor::DistToCPlayer()
{ return _ZN8dActor_c13DistToCPlayerEv(this); }

/* gate 42: PeachPainting's Behavior/Render call ModelBase::ApplyOpacity with a
   second argument (?ApplyOpacity@ModelBase@@QAEXIH@Z); the ROM body takes one,
   so the face drops it. */
struct ModelBase { void ApplyOpacity(unsigned int o, int x); };
void ModelBase::ApplyOpacity(unsigned int o, int)
{ _ZN9ModelBase12ApplyOpacityEj(this, o); }

/* ?UpdateWMClsn@dEnemyBase_c@@QAEXAAUWithMeshClsn@@I@Z */
struct WithMeshClsn;
struct Enemy { void UpdateWMClsn(WithMeshClsn &clsn, unsigned flags); };
void Enemy::UpdateWMClsn(WithMeshClsn &clsn, unsigned flags)
{ _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(this, &clsn, flags); }

/* ?Init@dCcAc_c@@QAEXPAUActor@@HHII@Z */
struct MovingCylinderClsn {
    void Init(Actor *actor, int radius, int height, unsigned e, unsigned f);
};
void MovingCylinderClsn::Init(Actor *actor, int radius, int height,
                              unsigned e, unsigned f)
{ _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(this, actor, radius,
                                                     height, e, f); }

/* ?SetFile@dBgW_KcMbg@@QAEXPAUKCL_File@@ABUMatrix4x3@@HFAAUCLPS_Block@@@Z */
struct MovingMeshCollider {
    void SetFile(KCL_File *file, const Matrix4x3 &mat, int scale, short angY,
                 CLPS_Block &clps);
};
void MovingMeshCollider::SetFile(KCL_File *file, const Matrix4x3 &mat,
                                 int scale, short angY, CLPS_Block &clps)
{
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        this, file, &mat, scale, angY, &clps);
}

/* ?Init@dBgCh_Actr@@QAEXPAUActor@@HHPAUVector3_16@@H@Z */
struct WithMeshClsn {
    void Init(Actor *actor, int radius, int height, Vector3_16 *v, int t);
    int IsOnWall() const;
    int JustHitGround() const;
    /* TouchesWater is DECLARED here and deliberately not defined -- the
       matched TU defines it. See the block further down. */
    int TouchesWater() const;
};
int WithMeshClsn::IsOnWall() const
{ return _ZNK10dBgCh_Actr8IsOnWallEv((void *)this); }
int WithMeshClsn::JustHitGround() const
{ return _ZNK10dBgCh_Actr13JustHitGroundEv((void *)this); }
void WithMeshClsn::Init(Actor *actor, int radius, int height, Vector3_16 *v,
                        int t)
{ _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(this, actor,
                                                             radius, height,
                                                             v, t); }

/* SignPost::InitResources reaches the two static loaders through placeholder
   names its own TU invented -- ModelLoadFile and MeshColliderLoadFile, both
   plain cdecl. Real definitions rather than aliases so the names stay
   readable in a map file. Both carry C linkage since main's sweep moved the
   declarations into the shared header's extern "C" block; the aliases cover
   any TU that still spells the C++ mangling. */
extern "C" void *ModelLoadFile(void *ptr)
{ return _ZN5Model8LoadFileER13SharedFilePtr(ptr); }
extern "C" void *MeshColliderLoadFile(void *ptr)
{ return _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(ptr); }
#pragma comment(linker, "/alternatename:?ModelLoadFile@@YAPAXPAX@Z=_ModelLoadFile")
#pragma comment(linker, "/alternatename:?MeshColliderLoadFile@@YAPAXPAX@Z=_MeshColliderLoadFile")

/* ?TrackInDeathTable@dActor_c@@QAEXXZ -- the 1-up's collect path. Its own TU
   defines it as a method over a locally-declared Actor; the C-name reference
   comes from a sibling .c.

   THIS USED TO RESTATE THE BODY rather than call it:

       { DeathTable_SetBit(*(short *)((char *)self + 0xce)); }

   which is exactly what src/_ZN8dActor_c17TrackInDeathTableEv.cpp does, that TU
   spelling the same 0xce as a named `deathTableId` field. Both were linked, so
   the linkage count read this symbol as decompiled code reaching the game
   while the code that actually ran was the line above. It was the only one of
   the 210 faces the wave-C adjudication found doing that
   (port/faces_adjudicated.txt); the rest were already real bridges. Now it is
   one too, and the matched body is what runs. */
extern "C" void _ZN8dActor_c17TrackInDeathTableEv(void *self)
{ ((Actor *)self)->Actor::TrackInDeathTable(); }

/* ?InitCuboid@ShadowModel@@QAEXXZ IS GONE FROM THIS FILE (run linkw wave 3,
   lane w3-a). It used to be a local `struct ShadowModel { void InitCuboid(); }`
   whose method forwarded to the C name, which at the time was a no-op in
   hal/cxxname_bridge.cpp -- so SIGN_POST asked for its cuboid shadow and got
   nothing.

   That declaration decorated to exactly the same symbol the matched TU
   defines, and slice_w1l3.txt recorded it as the reason
   src/_ZN11ShadowModel10InitCuboidEv.cpp could not be taken in wave 2 ("that
   file belongs to no lane this wave, so the duplicate cannot be cleared").
   This wave owns the file, so the duplicate is cleared by deletion and the
   matched body owns the name. SIGN_POST's call site is unchanged and now
   reaches the ROM's own InitCuboid, which does SetFile(&data_020ad524, 1, -1)
   against the ov001 bytes port/ov001_syms.txt mounts.

   The C name keeps a definition -- it is now the ordinary bridge in the other
   direction (C name -> matched method), in hal/cxxname_bridge.cpp. */

/* WithMeshClsn::TouchesWater is declared only inside its own TU (not in
   include/WithMeshClsn.h), and the sign's thrown and dropped states call it by
   its Itanium name.

   THE MATCHED BODY OWNS THE WORK NOW (run linkw wave 3, w3-a). The host body
   here read SurfaceInfo_TestFlag0x20(this + 0x34) directly;
   src/_ZNK10dBgCh_Actr12TouchesWaterEv.cpp calls the same helper at the same
   offset, so this is a swap with no behaviour change to argue about -- the
   only difference is which object file the instructions come from. The TU was
   already compiled (slice_gate16.txt) and stripped for want of a reference, so
   this C face IS the reference edge and no slice line is needed. 5 of its 7
   ROM callers are in the image.

   TouchesWater is declared on the WithMeshClsn shadow ABOVE rather than on a
   fresh local one. A second `struct WithMeshClsn` in this file would be a
   redefinition -- this file includes no headers, but it already declares that
   class at the Init/IsOnWall/JustHitGround face. MSVC decorates a method off
   the class NAME and signature only, so ?TouchesWater@dBgCh_Actr@@QBEHXZ
   from that declaration is the symbol the matched TU exports; no layout is
   assumed and the `pad` the matched TU carries is not repeated.

   SurfaceInfo_TestFlag0x20 is no longer named here. The matched body calls it
   with the same `this + 0x34`, so the reference simply moved into the TU that
   does the work. */
extern "C" int _ZNK10dBgCh_Actr12TouchesWaterEv(const void *self)
{ return ((const WithMeshClsn *)self)->WithMeshClsn::TouchesWater(); }
