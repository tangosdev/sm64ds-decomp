//cpp
extern "C" {
typedef struct { int x, y, z; } Vector3;
typedef struct dBgCh_Actr dBgCh_Actr;
/* NOT SurfaceInfo. This is the 0x24 remainder of dBgPi after its vptr
   (0x28 = 4 + 0x24), and it was the only definition in the tree claiming a
   size other than 0x14 for a type called SurfaceInfo. Renamed 2026-08-19 so
   the name means one thing; the real one is include/SurfaceInfo.h. */
struct dBgPiBody {
  int a, b, c, d, e;
  unsigned short f, g;
  int h, i, j;
};
struct dBgPi {
  void* vt;
  struct dBgPiBody info;
};
void dBgCh_Actr_UpdateDiscreteNoLava_veneer(void* w);
int _ZNK10dBgCh_Actr10IsOnGroundEv(dBgCh_Actr* w);
void* _ZNK10dBgCh_Actr14GetFloorResultEv(dBgCh_Actr* w);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* s, Vector3* v);
int _ZN4cstd4fdivEii(int a, int b);
int _ZNK10dBgCh_Actr8IsOnWallEv(dBgCh_Actr* w);
struct dBgPi* _ZNK10dBgCh_Actr13GetWallResultEv(dBgCh_Actr* w);
void _ZN5dBgPiD1Ev(struct dBgPi* r);
extern void* data_02099368;

void func_ov018_02111bf0(char* c, dBgCh_Actr* w){
  dBgCh_Actr_UpdateDiscreteNoLava_veneer(w);
  if (_ZNK10dBgCh_Actr10IsOnGroundEv(w) != 0) {
    Vector3 n;
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3(((char*)_ZNK10dBgCh_Actr14GetFloorResultEv(w) + 4), &n);
    if (n.y != 0) {
      int s = (int)(((long long)n.x * *(int*)(c+0xa4) + 0x800) >> 0xc)
            + (int)(((long long)n.z * *(int*)(c+0xac) + 0x800) >> 0xc);
      *(int*)(c+0xa8) = -(_ZN4cstd4fdivEii(s, n.y) + 0x8000);
    }
  }
  if (_ZNK10dBgCh_Actr8IsOnWallEv(w) != 0) {
    struct dBgPi* src = _ZNK10dBgCh_Actr13GetWallResultEv(w);
    struct dBgPi cr;
    Vector3 wn;
    struct dBgPiBody* dst = &cr.info;
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
    _ZN5dBgPiD1Ev(&cr);
  }
}
}
