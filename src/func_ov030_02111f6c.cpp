//cpp
extern "C" {
typedef struct { int x, y, z; } Vector3;
typedef struct WithMeshClsn WithMeshClsn;
typedef struct SurfaceInfo SurfaceInfo;
void func_020383f0(void* p);
void func_020383fc(void* p);
int _ZNK12WithMeshClsn10IsOnGroundEv(WithMeshClsn* w);
void* _ZNK12WithMeshClsn14GetFloorResultEv(WithMeshClsn* w);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(SurfaceInfo* s, Vector3* v);
int _ZN4cstd4fdivEii(int a, int b);
int _ZNK12WithMeshClsn8IsOnWallEv(WithMeshClsn* w);
void* _ZNK12WithMeshClsn13GetWallResultEv(WithMeshClsn* w);

void func_ov030_02111f6c(char* c, WithMeshClsn* w){
  int b = (int)((*(int*)(c+0xb0) & 0x4000) != 0);
  if (b != 0) return;
  int bb = (int)(*(unsigned short*)(c+0xc) == 0x10b);
  if (bb != 0 && *(int*)(c+0x3b4) != 9) func_020383f0(c+0x194);
  else func_020383fc(c+0x194);
  if (_ZNK12WithMeshClsn10IsOnGroundEv(w) != 0) {
    Vector3 n;
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3((SurfaceInfo*)((char*)_ZNK12WithMeshClsn14GetFloorResultEv(w) + 4), &n);
    if (n.y != 0) {
      int s = (int)(((long long)n.x * *(int*)(c+0xa4) + 0x800) >> 0xc)
            + (int)(((long long)n.z * *(int*)(c+0xac) + 0x800) >> 0xc);
      *(int*)(c+0xa8) = -(_ZN4cstd4fdivEii(s, n.y) + 0x8000);
    }
  }
  if (_ZNK12WithMeshClsn8IsOnWallEv(w) != 0) {
    Vector3 wn;
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3((SurfaceInfo*)((char*)_ZNK12WithMeshClsn13GetWallResultEv(w) + 4), &wn);
  }
}
}
