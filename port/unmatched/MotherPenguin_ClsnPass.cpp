/* HOST COPY of src/func_ov018_02111bf0.c -- MOTHER_PENGUIN's floor/wall
 * collision pass, under a PORT NAME. Run link100, lane MPG, gate mpg.
 *
 * WHY THE NAME CHANGES, and it is the only change. The symbol
 * func_ov018_02111bf0 is ALREADY SPOKEN FOR in this binary, by gate 188:
 * port/unmatched/Jrb_Aliases.cpp:34 carries
 *     /alternatename:_func_ov018_02111bf0=_func_ov016_02111bf0
 * because three UNAGI bodies in ov016 -- func_ov016_02111534,
 * func_ov016_02111758 and func_ov016_021118b4, all three linked today -- call
 * "func_ov018_02111bf0" where their own module's 0x02111bf0 is meant (ov016
 * and ov018 share load base 0x021111a0). Gate 188 called that a cross-overlay
 * typo in the recovered src and bridged the ov018 spelling to ov016's body.
 *
 * ov018's 0x02111bf0 is a REAL, DIFFERENT function, and this gate's own
 * func_ov018_02112234 and func_ov018_02111e28 call it -- ov018's relocs.txt
 * says overlay(18) at both sites. So slicing src/func_ov018_02111bf0.c defines
 * the alias's LHS, DEFEATS the alias, and silently reroutes Unagi's three
 * bodies into the mother penguin's collision pass. That is not a theory:
 * port/tools/alternatename_guard.py failed this gate's build on exactly that,
 * naming LHS 0001:00188350 against RHS 0001:00148d00.
 *
 * THE HONEST FIX for the whole knot is the guard's own R1/R2 recipe -- delete
 * the dead alias and compile the three ov016 TUs with a per-source
 * -Dfunc_ov018_02111bf0=func_ov016_02111bf0 -- but that edits
 * port/unmatched/Jrb_Aliases.cpp, which is outside this lane's owned files.
 * So this gate takes the version that stays inside them and cannot regress
 * Jolly Roger Bay: the body is host copied under port_mpg_clsn_pass, the
 * alias is left untouched and keeps firing for Unagi, and this gate's two
 * callers are renamed onto the host name in the CMake gate block. The matched
 * src TU stays in src/ as the byte proof and is NOT sliced -- which is why
 * this gate links nine ov018 TUs and not ten. The tenth is a one-line
 * proposal in this lane's report.
 *
 * Matched source line for line otherwise; only the function's own name
 * changes.
 */
extern "C" {
typedef struct { int x, y, z; } Vector3;
typedef struct WithMeshClsn WithMeshClsn;
struct SurfaceInfo {
  int a, b, c, d, e;
  unsigned short f, g;
  int h, i, j;
};
struct ClsnResult {
  void* vt;
  struct SurfaceInfo info;
};
void WithMeshClsn_UpdateDiscreteNoLava_veneer(void* w);
int _ZNK12WithMeshClsn10IsOnGroundEv(WithMeshClsn* w);
void* _ZNK12WithMeshClsn14GetFloorResultEv(WithMeshClsn* w);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(struct SurfaceInfo* s, Vector3* v);
int _ZN4cstd4fdivEii(int a, int b);
int _ZNK12WithMeshClsn8IsOnWallEv(WithMeshClsn* w);
struct ClsnResult* _ZNK12WithMeshClsn13GetWallResultEv(WithMeshClsn* w);
void _ZN10ClsnResultD1Ev(struct ClsnResult* r);
extern void* data_02099368;

void port_mpg_clsn_pass(char* c, WithMeshClsn* w){
  WithMeshClsn_UpdateDiscreteNoLava_veneer(w);
  if (_ZNK12WithMeshClsn10IsOnGroundEv(w) != 0) {
    Vector3 n;
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3((struct SurfaceInfo*)((char*)_ZNK12WithMeshClsn14GetFloorResultEv(w) + 4), &n);
    if (n.y != 0) {
      int s = (int)(((long long)n.x * *(int*)(c+0xa4) + 0x800) >> 0xc)
            + (int)(((long long)n.z * *(int*)(c+0xac) + 0x800) >> 0xc);
      *(int*)(c+0xa8) = -(_ZN4cstd4fdivEii(s, n.y) + 0x8000);
    }
  }
  if (_ZNK12WithMeshClsn8IsOnWallEv(w) != 0) {
    struct ClsnResult* src = _ZNK12WithMeshClsn13GetWallResultEv(w);
    struct ClsnResult cr;
    Vector3 wn;
    struct SurfaceInfo* dst = &cr.info;
    // demand a first (should get r4), then b (r1), then dst (r2)
    int a = *(int*)((char*)src + 4);
    int b = *(int*)((char*)src + 8);
    *(int*)((char*)dst + 0) = b ? a : a;
    *(int*)((char*)dst + 4) = b;
    int t = *(int*)((char*)src + 0xc);
    void* vt = &data_02099368;
    *(int*)((char*)dst + 8) = t;
    t = *(int*)((char*)src + 0x10);
    *(int*)((char*)dst + 0xc) = t;
    t = *(int*)((char*)src + 0x14);
    *(int*)((char*)dst + 0x10) = t;
    cr.vt = vt;
    // remaining via cr members for sp-relative
    cr.info.f = *(unsigned short*)((char*)src + 0x18);
    cr.info.g = *(unsigned short*)((char*)src + 0x1a);
    cr.info.h = *(int*)((char*)src + 0x1c);
    cr.info.i = *(int*)((char*)src + 0x20);
    cr.info.j = *(int*)((char*)src + 0x24);
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3(dst, &wn);
    _ZN10ClsnResultD1Ev(&cr);
  }
}
}
