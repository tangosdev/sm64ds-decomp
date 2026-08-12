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
struct CylinderClsn;
struct Vector3_16;
struct KCL_File;
struct Matrix4x3;
struct CLPS_Block;
struct SharedFilePtr;

extern "C" {
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *clsn);
void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *self, void *actor,
                                                        int r, int h,
                                                        unsigned e,
                                                        unsigned f);
void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, const void *mat, int scale, short angY, void *clps);
void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(
    void *self, void *actor, int r, int h, void *v, int t);
void _ZN5Actor28UpdatePosWithHorzSpeedAndAngEv(void *self);
short _ZN5Actor12ReflectAngleE5Fix12IiES1_s(void *self, int a, int b, short c);
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, void *clsn,
                                              unsigned flags);
int _ZNK12WithMeshClsn8IsOnWallEv(void *self);
int _ZNK12WithMeshClsn13JustHitGroundEv(void *self);
void *_ZN5Model8LoadFileER13SharedFilePtr(void *ptr);
void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(void *ptr);
/* gate 42: the two methods PeachPainting's Behavior reaches by shadow name.
   DistToCPlayer is C-named in src (slice_gate16); the ROM's ApplyOpacity is a
   one-argument body (src/_ZN9ModelBase12ApplyOpacityEj.cpp), but the painting's
   call passes the ROM's r2=1 as a second argument the body ignores, so the face
   forwards only the opacity, the gate-16 reading of the same method. */
int _ZN5Actor13DistToCPlayerEv(void *self);
void _ZN9ModelBase12ApplyOpacityEj(void *self, unsigned a);
}

/* Actor: three methods the 1-up's type bodies and the sign's thrown state
   reach.
     ?UpdatePos@Actor@@QAEXPAUCylinderClsn@@@Z
     ?UpdatePosWithHorzSpeedAndAng@Actor@@QAEXXZ
     ?ReflectAngle@Actor@@QAEFHHF@Z  (the C definition spells the two Fix12i
       arguments as the template type, which mangles differently -- same
       function, and the reloc in the caller is what settles it) */
struct Actor {
    void UpdatePos(CylinderClsn *clsn);
    void UpdatePosWithHorzSpeedAndAng();
    short ReflectAngle(int a, int b, short c);
    int DistToCPlayer();          /* gate 42: PeachPainting::Behavior */
};
#ifdef _WIN32 /* LINUX: these methods ARE their own manglings -> self-recurse; real src TUs own the symbols. (ReflectAngle stays -- it forwards to the DIFFERENTLY-mangled Fix12 template symbol, a real bridge on both.) */
void Actor::UpdatePos(CylinderClsn *clsn)
{ _ZN5Actor9UpdatePosEP12CylinderClsn(this, clsn); }
void Actor::UpdatePosWithHorzSpeedAndAng()
{ _ZN5Actor28UpdatePosWithHorzSpeedAndAngEv(this); }
#endif
short Actor::ReflectAngle(int a, int b, short c)
{ return _ZN5Actor12ReflectAngleE5Fix12IiES1_s(this, a, b, c); }
#ifdef _WIN32 /* LINUX: DistToCPlayer IS its own mangling -> self-recurse. */
int Actor::DistToCPlayer()
{ return _ZN5Actor13DistToCPlayerEv(this); }
#endif

/* gate 42: PeachPainting's Behavior/Render call ModelBase::ApplyOpacity with a
   second argument (?ApplyOpacity@ModelBase@@QAEXIH@Z); the ROM body takes one,
   so the face drops it. */
struct ModelBase { void ApplyOpacity(unsigned int o, int x); };
void ModelBase::ApplyOpacity(unsigned int o, int)
{ _ZN9ModelBase12ApplyOpacityEj(this, o); }

/* ?UpdateWMClsn@Enemy@@QAEXAAUWithMeshClsn@@I@Z */
struct WithMeshClsn;
struct Enemy { void UpdateWMClsn(WithMeshClsn &clsn, unsigned flags); };
void Enemy::UpdateWMClsn(WithMeshClsn &clsn, unsigned flags)
{ _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(this, &clsn, flags); }

/* ?Init@MovingCylinderClsn@@QAEXPAUActor@@HHII@Z */
struct MovingCylinderClsn {
    void Init(Actor *actor, int radius, int height, unsigned e, unsigned f);
};
void MovingCylinderClsn::Init(Actor *actor, int radius, int height,
                              unsigned e, unsigned f)
{ _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(this, actor, radius,
                                                     height, e, f); }

/* ?SetFile@MovingMeshCollider@@QAEXPAUKCL_File@@ABUMatrix4x3@@HFAAUCLPS_Block@@@Z */
struct MovingMeshCollider {
    void SetFile(KCL_File *file, const Matrix4x3 &mat, int scale, short angY,
                 CLPS_Block &clps);
};
void MovingMeshCollider::SetFile(KCL_File *file, const Matrix4x3 &mat,
                                 int scale, short angY, CLPS_Block &clps)
{
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        this, file, &mat, scale, angY, &clps);
}

/* ?Init@WithMeshClsn@@QAEXPAUActor@@HHPAUVector3_16@@H@Z */
struct WithMeshClsn {
    void Init(Actor *actor, int radius, int height, Vector3_16 *v, int t);
    int IsOnWall() const;
    int JustHitGround() const;
};
#ifdef _WIN32 /* LINUX: these const methods ARE their own manglings -> self-recurse; real src TUs own them. */
int WithMeshClsn::IsOnWall() const
{ return _ZNK12WithMeshClsn8IsOnWallEv((void *)this); }
int WithMeshClsn::JustHitGround() const
{ return _ZNK12WithMeshClsn13JustHitGroundEv((void *)this); }
#endif
void WithMeshClsn::Init(Actor *actor, int radius, int height, Vector3_16 *v,
                        int t)
{ _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(this, actor,
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
{ return _ZN12MeshCollider8LoadFileER13SharedFilePtr(ptr); }
#pragma comment(linker, "/alternatename:?ModelLoadFile@@YAPAXPAX@Z=_ModelLoadFile")
#pragma comment(linker, "/alternatename:?MeshColliderLoadFile@@YAPAXPAX@Z=_MeshColliderLoadFile")

/* ?TrackInDeathTable@Actor@@QAEXXZ -- the 1-up's collect path. Its own TU
   defines it as a method over a locally-declared Actor; the C-name reference
   comes from a sibling .c. */
extern "C" void DeathTable_SetBit(int id);
extern "C" void _ZN5Actor17TrackInDeathTableEv(void *self)
{ DeathTable_SetBit(*(short *)((char *)self + 0xce)); }

/* ?InitCuboid@ShadowModel@@QAEXXZ -- SIGN_POST asks for its cuboid shadow
   through a method on its own shadow declaration, and the shadow system is
   deferred: data_020ad524, the template BMD, is static .data in overlay 1 and
   the port never mounts ov001, so the real body parses a zero stub (NOT a
   missing runtime builder, as this comment used to say; cxxname_bridge.cpp
   carries the corrected writeup). Forward to the same no-op every C-named
   caller already gets. The sign is drawn without its shadow until ov001's
   data is mounted. */
extern "C" void _ZN11ShadowModel10InitCuboidEv(void *self);
struct ShadowModel { void InitCuboid(); };
void ShadowModel::InitCuboid() { _ZN11ShadowModel10InitCuboidEv(this); }

/* WithMeshClsn::TouchesWater is declared only inside its own TU (not in
   include/WithMeshClsn.h), and the sign's thrown and dropped states call it by
   its Itanium name. The C face is the mirror of the shadow-class ones above:
   this side declares the method, forwards to the definition. */
extern "C" int _ZNK12WithMeshClsn12TouchesWaterEv(const void *self);
extern "C" int SurfaceInfo_TestFlag0x20(const void *p);
extern "C" int _ZNK12WithMeshClsn12TouchesWaterEv(const void *self)
{ return SurfaceInfo_TestFlag0x20((const char *)self + 0x34); }
