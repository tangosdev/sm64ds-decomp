//cpp
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
void func_02038420(void* w);
int _ZNK12WithMeshClsn10IsOnGroundEv(WithMeshClsn* w);
void* _ZNK12WithMeshClsn14GetFloorResultEv(WithMeshClsn* w);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(struct SurfaceInfo* s, Vector3* v);
int _ZN4cstd4fdivEii(int a, int b);
int _ZNK12WithMeshClsn8IsOnWallEv(WithMeshClsn* w);
struct ClsnResult* _ZNK12WithMeshClsn13GetWallResultEv(WithMeshClsn* w);
void _ZN10ClsnResultD1Ev(struct ClsnResult* r);
extern void* data_02099368;

void func_ov018_02111bf0(char* c, WithMeshClsn* w){
  func_02038420(w);
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
