// Seat the REAL MeshColliderBase vtable (_ZTV16MeshColliderBase, arm9
// 0x02099388) with its own matched ROM bodies, the faithful class seat in the
// QUESTION_BLOCK sense: hal_fill_meshcolliderbase_vtable NAMES the matched src
// bodies, so the linker pulls the whole MeshColliderBase cluster (its ten own
// methods) into the closure. That fill is the reference edge.
//
// STORAGE vs SEAT. The array itself lives in hal/model_host.cpp, because the
// base ctor MeshColliderBase::MeshColliderBase (C2, 0x0203969c) installs it as
// the vptr and C2 rides slice_gate8 -- linked by every collision target,
// including the smoke ones that carry no MeshColliderBase method body. This
// file is added only to the three targets that also carry slice_gate16 (the
// MeshColliderBase cluster): smoke_player, walk_window, walk_window_hires. The
// runtime fill is the same mechanism hal_fill_moving_mesh_collider_vtable uses
// for the derived table; level_boot calls it at boot. Where the cluster is not
// linked, the base table stays the zeroed model_host storage and is never
// dispatched (the header's note; the concrete tables in clsn_vtable.cpp carry
// the live dispatch).
//
// SLOT ORDER IS THE ROM'S (Itanium), read out of config's relocs for the
// vtable span 0x02099388..0x020993bc and cross-checked against
// include/MeshColliderBase.h. Slots 3/4/5 are PURE VIRTUAL in the base
// (GetSurfaceInfo/GetNormal/GetTriangleOrigin, null in the ROM table, filled
// only in derived tables): they stay null.
//
// TWO NAME SPACES. The two destructor bodies are C-linkage src (.c, flat
// _ZN.. symbols). The other eight are real MSVC methods (their .cpp TUs define
// MeshColliderBase::Method), so the flat extern-C decl here is bridged to the
// MSVC-mangled name with an /alternatename, the mechanism hal/cxx_aliases.cpp
// already uses for this class's Enable and its three UpdatePos statics. The
// mangled targets were read out of the compiled objs (dumpbin /SYMBOLS).
//
//   slot  0  ~MeshColliderBase D1   0x0203968c (config spells it D2Ev; #774)
//   slot  1  ~MeshColliderBase D0   0x02039668
//   slot  2  Virtual08              0x02039620
//   slot  6  DetectClsn(RaycastGround &)  0x02039480
//   slot  7  DetectClsn(RaycastLine &)    0x02039478
//   slot  8  DetectClsn(SphereClsn &)     0x02039470
//   slot  9  BeforeClsn(...)         0x02039438
//   slot 10  TransformPos(...)       0x02039430
//   slot 11  GetAngularVelY()        0x02039428
//   slot 12  GetVelocity(Vector3 &)  0x0203940c

// Flat C name -> the MSVC-mangled method the matched .cpp TU actually emits.
#pragma comment(linker, "/alternatename:__ZN16MeshColliderBase9Virtual08Ev=?Virtual08@MeshColliderBase@@UAEXXZ")
#pragma comment(linker, "/alternatename:__ZN16MeshColliderBase10DetectClsnER13RaycastGround=?DetectClsn@MeshColliderBase@@UAEHAAURaycastGround@@@Z")
#pragma comment(linker, "/alternatename:__ZN16MeshColliderBase10DetectClsnER11RaycastLine=?DetectClsn@MeshColliderBase@@UAEHAAURaycastLine@@@Z")
#pragma comment(linker, "/alternatename:__ZN16MeshColliderBase10DetectClsnER10SphereClsn=?DetectClsn@MeshColliderBase@@UAEHAAUSphereClsn@@@Z")
#pragma comment(linker, "/alternatename:__ZN16MeshColliderBase10BeforeClsnER10ClsnResultP5ActorR7Vector3P10Vector3_16S7_=?BeforeClsn@MeshColliderBase@@UAEXAAUClsnResult@@PAUActor@@AAUVector3@@PAUVector3_16@@3@Z")
#pragma comment(linker, "/alternatename:__ZN16MeshColliderBase12TransformPosERK7Vector3RS0_=?TransformPos@MeshColliderBase@@UAEHABUVector3@@AAU2@@Z")
#pragma comment(linker, "/alternatename:__ZN16MeshColliderBase14GetAngularVelYEv=?GetAngularVelY@MeshColliderBase@@UAEFXZ")
#pragma comment(linker, "/alternatename:__ZN16MeshColliderBase11GetVelocityER7Vector3=?GetVelocity@MeshColliderBase@@UAEXAAUVector3@@@Z")

extern "C" {

extern void *_ZTV16MeshColliderBase[13];   /* storage in hal/model_host.cpp */

void *_ZN16MeshColliderBaseD2Ev(void *self);   /* slot 0 body (D1 by #774) */
void *_ZN16MeshColliderBaseD0Ev(void *self);   /* slot 1 */
void _ZN16MeshColliderBase9Virtual08Ev(void *self);
int _ZN16MeshColliderBase10DetectClsnER13RaycastGround(void *self, void *ray);
int _ZN16MeshColliderBase10DetectClsnER11RaycastLine(void *self, void *ray);
int _ZN16MeshColliderBase10DetectClsnER10SphereClsn(void *self, void *sphere);
void _ZN16MeshColliderBase10BeforeClsnER10ClsnResultP5ActorR7Vector3P10Vector3_16S7_(
    void *self, void *res, void *actor, void *pos, void *motionAng, void *ang);
int _ZN16MeshColliderBase12TransformPosERK7Vector3RS0_(void *self, const void *pos, void *res);
short _ZN16MeshColliderBase14GetAngularVelYEv(void *self);
void _ZN16MeshColliderBase11GetVelocityER7Vector3(void *self, void *res);

/* Called from level_boot's boot fills. Naming the ten matched bodies here is
   the reference edge that pulls the MeshColliderBase cluster into the link. */
void hal_fill_meshcolliderbase_vtable(void)
{
    void **vt = _ZTV16MeshColliderBase;
    vt[0]  = (void *)_ZN16MeshColliderBaseD2Ev;
    vt[1]  = (void *)_ZN16MeshColliderBaseD0Ev;
    vt[2]  = (void *)_ZN16MeshColliderBase9Virtual08Ev;
    vt[3]  = (void *)0;   /* pure GetSurfaceInfo */
    vt[4]  = (void *)0;   /* pure GetNormal */
    vt[5]  = (void *)0;   /* pure GetTriangleOrigin */
    vt[6]  = (void *)_ZN16MeshColliderBase10DetectClsnER13RaycastGround;
    vt[7]  = (void *)_ZN16MeshColliderBase10DetectClsnER11RaycastLine;
    vt[8]  = (void *)_ZN16MeshColliderBase10DetectClsnER10SphereClsn;
    vt[9]  = (void *)_ZN16MeshColliderBase10BeforeClsnER10ClsnResultP5ActorR7Vector3P10Vector3_16S7_;
    vt[10] = (void *)_ZN16MeshColliderBase12TransformPosERK7Vector3RS0_;
    vt[11] = (void *)_ZN16MeshColliderBase14GetAngularVelYEv;
    vt[12] = (void *)_ZN16MeshColliderBase11GetVelocityER7Vector3;
}

} /* extern "C" */
