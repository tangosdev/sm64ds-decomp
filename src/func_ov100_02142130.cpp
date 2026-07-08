//cpp
extern "C" {
typedef struct { int x, y, z; } Vector3;
typedef struct WithMeshClsn WithMeshClsn;
typedef struct SurfaceInfo SurfaceInfo;
typedef struct Actor Actor;
void func_020383f0(void* p);
void func_02038414(void* p);
int _ZNK12WithMeshClsn8IsOnWallEv(WithMeshClsn* w);
void* _ZNK12WithMeshClsn13GetWallResultEv(WithMeshClsn* w);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(SurfaceInfo* s, Vector3* v);
int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
int _ZN5Actor14GetSubtractionEss(Actor* a, short s1, int s2);
int _ZNK12WithMeshClsn10IsOnGroundEv(WithMeshClsn* w);
void* _ZNK12WithMeshClsn14GetFloorResultEv(WithMeshClsn* w);
int _ZN4cstd4fdivEii(int a, int b);
extern signed char LEVEL_ID;

void func_ov100_02142130(char* c){
  *(unsigned char*)(c+0x3d1) = 0;
  if (LEVEL_ID == 0x19) func_020383f0(c+0x110);
  else func_02038414(c+0x110);
  if (_ZNK12WithMeshClsn8IsOnWallEv((WithMeshClsn*)(c+0x110)) != 0) {
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3((SurfaceInfo*)((char*)_ZNK12WithMeshClsn13GetWallResultEv((WithMeshClsn*)(c+0x110)) + 4), (Vector3*)(c+0xe0));
    int a = _ZN4cstd5atan2E5Fix12IiES1_(*(int*)(c+0xe0), *(int*)(c+0xe8));
    if (_ZN5Actor14GetSubtractionEss((Actor*)c, *(short*)(c+0x94), a) > 0x4000)
      *(unsigned char*)(c+0x3d1) = 1;
  }
  if (_ZNK12WithMeshClsn10IsOnGroundEv((WithMeshClsn*)(c+0x110)) == 0) return;
  _ZNK11SurfaceInfo12CopyNormalToER7Vector3((SurfaceInfo*)((char*)_ZNK12WithMeshClsn14GetFloorResultEv((WithMeshClsn*)(c+0x110)) + 4), (Vector3*)(c+0xd4));
  if (*(unsigned char*)(c+0x3d1) != 0) return;
  *(int*)(c+0x5c) = *(int*)(c+0x68);
  *(int*)(c+0x64) = *(int*)(c+0x70);
  if (*(int*)(c+0xd8) == 0) return;
  int s = (int)(((long long)*(int*)(c+0xd4) * *(int*)(c+0xa4) + 0x800) >> 0xc)
        + (int)(((long long)*(int*)(c+0xdc) * *(int*)(c+0xac) + 0x800) >> 0xc);
  *(int*)(c+0xa8) = -(_ZN4cstd4fdivEii(s, *(int*)(c+0xd8)) + 0x8000);
}
}
