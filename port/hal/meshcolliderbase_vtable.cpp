// Seat the REAL MeshColliderBase vtable (_ZTV4dBgW, arm9
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
#pragma comment(linker, "/alternatename:__ZN4dBgW9Virtual08Ev=?Virtual08@dBgW@@UAEXXZ")
// FOUR ROWS RETIRED at SMOKELINK (wave 10). All four are dead in the shape
// alternatename_guard refuses: the LHS is a DEFINED symbol, so the alias is
// inert and references bind to that definition rather than to the RHS, and in
// these four the RHS is not in the map at all -- the 09-14 main-to-port sync
// renamed the parameter classes (RaycastGround/RaycastLine/SphereClsn ->
// dBgCh_Gnd/dBgCh_Lin/dBgCh_SphCrr, ClsnResult -> dBgPi), so the decorated
// spellings these rows name stopped existing. The flat names are defined by
// the sync FACES instead, which walk_window.map carries:
//   0001:001c6190  __ZN4dBgW10DetectClsnER9dBgCh_Gnd      faces_sync_gen.cpp.obj
//   0001:001c6190  __ZN4dBgW10DetectClsnER9dBgCh_Lin      faces_sync_gen.cpp.obj
//   0001:001c6190  __ZN4dBgW10DetectClsnER12dBgCh_SphCrr  faces_sync_gen.cpp.obj
//   0001:001c6170  __ZN4dBgW10BeforeClsnER5dBgPi...       faces_sync_gen.cpp.obj
// A face is the right binding here and an alias never was: the flat caller is
// __cdecl with the receiver on the stack and the synced body is __thiscall
// with it in ECX, which is a NAME bridge against an ABI mismatch.
// NOT FIXED HERE, and logged rather than touched: those three DetectClsn faces
// share ONE address while the cartridge has three distinct virtuals at
// 0x02039480 / 0x02039478 / 0x02039470 (see out/SMOKELINK/bugs.md, and lane
// SINGLES's bugs.md item 3, which found it first). That belongs to whoever
// owns port/faces_sync.txt.
// #pragma comment(linker, "/alternatename:__ZN4dBgW10DetectClsnER9dBgCh_Gnd=?DetectClsn@dBgW@@UAEHAAURaycastGround@@@Z")
// #pragma comment(linker, "/alternatename:__ZN4dBgW10DetectClsnER9dBgCh_Lin=?DetectClsn@dBgW@@UAEHAAURaycastLine@@@Z")
// #pragma comment(linker, "/alternatename:__ZN4dBgW10DetectClsnER12dBgCh_SphCrr=?DetectClsn@dBgW@@UAEHAAUSphereClsn@@@Z")
// #pragma comment(linker, "/alternatename:__ZN4dBgW10BeforeClsnER5dBgPiP8dActor_cR7Vector3P10Vector3_16S7_=?BeforeClsn@dBgW@@UAEXAAUClsnResult@@PAUActor@@AAUVector3@@PAUVector3_16@@3@Z")
#pragma comment(linker, "/alternatename:__ZN4dBgW12TransformPosERK7Vector3RS0_=?TransformPos@dBgW@@UAEHABUVector3@@AAU2@@Z")
#pragma comment(linker, "/alternatename:__ZN4dBgW14GetAngularVelYEv=?GetAngularVelY@dBgW@@UAEFXZ")
#pragma comment(linker, "/alternatename:__ZN4dBgW11GetVelocityER7Vector3=?GetVelocity@dBgW@@UAEXAAUVector3@@@Z")

extern "C" {

extern void *_ZTV4dBgW[13];   /* storage in hal/model_host.cpp */

void *_ZN4dBgWD1Ev(void *self);   /* slot 0 body (D1 by #774) */
void *_ZN4dBgWD0Ev(void *self);   /* slot 1 */
void _ZN4dBgW9Virtual08Ev(void *self);
int _ZN4dBgW10DetectClsnER9dBgCh_Gnd(void *self, void *ray);
int _ZN4dBgW10DetectClsnER9dBgCh_Lin(void *self, void *ray);
int _ZN4dBgW10DetectClsnER12dBgCh_SphCrr(void *self, void *sphere);
void _ZN4dBgW10BeforeClsnER5dBgPiP8dActor_cR7Vector3P10Vector3_16S7_(
    void *self, void *res, void *actor, void *pos, void *motionAng, void *ang);
int _ZN4dBgW12TransformPosERK7Vector3RS0_(void *self, const void *pos, void *res);
short _ZN4dBgW14GetAngularVelYEv(void *self);
void _ZN4dBgW11GetVelocityER7Vector3(void *self, void *res);

/* Called from level_boot's boot fills. Naming the ten matched bodies here is
   the reference edge that pulls the MeshColliderBase cluster into the link. */
void hal_fill_meshcolliderbase_vtable(void)
{
    void **vt = _ZTV4dBgW;
    vt[0]  = (void *)_ZN4dBgWD1Ev;
    vt[1]  = (void *)_ZN4dBgWD0Ev;
    vt[2]  = (void *)_ZN4dBgW9Virtual08Ev;
    vt[3]  = (void *)0;   /* pure GetSurfaceInfo */
    vt[4]  = (void *)0;   /* pure GetNormal */
    vt[5]  = (void *)0;   /* pure GetTriangleOrigin */
    vt[6]  = (void *)_ZN4dBgW10DetectClsnER9dBgCh_Gnd;
    vt[7]  = (void *)_ZN4dBgW10DetectClsnER9dBgCh_Lin;
    vt[8]  = (void *)_ZN4dBgW10DetectClsnER12dBgCh_SphCrr;
    vt[9]  = (void *)_ZN4dBgW10BeforeClsnER5dBgPiP8dActor_cR7Vector3P10Vector3_16S7_;
    vt[10] = (void *)_ZN4dBgW12TransformPosERK7Vector3RS0_;
    vt[11] = (void *)_ZN4dBgW14GetAngularVelYEv;
    vt[12] = (void *)_ZN4dBgW11GetVelocityER7Vector3;
}

} /* extern "C" */
