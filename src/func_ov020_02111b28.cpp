//cpp
extern "C" {
void _ZN9Animation7AdvanceEv(void* self);
void AddVec3(void* a, void* b, void* c);
int func_ov020_02111418(char* c);
void func_020383fc(void* c);
int _ZNK12WithMeshClsn10IsOnGroundEv(void* self);
void func_ov020_02112110(char* c);
int _ZNK12WithMeshClsn8IsOnWallEv(void* self);
int _ZNK12WithMeshClsn13GetWallResultEv(void* self);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* self, void* v);
int _ZN4cstd5atan2E5Fix12IiES1_(int x, int y);
short _ZN5Actor14GetSubtractionEss(void* self, short a, int b);
void func_0201267c(int a, void* p);

void func_ov020_02111b28(char* c){
  if (*(unsigned char*)(c+0x450) != 0) {
    _ZN9Animation7AdvanceEv(c+0x160);
  }
  AddVec3(c+0x5c, c+0xa4, c+0x5c);
  if (func_ov020_02111418(c) != 0) return;
  func_020383fc(c+0x25c);
  if (_ZNK12WithMeshClsn10IsOnGroundEv(c+0x25c) != 0) {
    *(unsigned char*)(c+0x108) = 0;
    func_ov020_02112110(c);
    func_0201267c(0xc5, c+0x74);
    return;
  }
  if (_ZNK12WithMeshClsn8IsOnWallEv(c+0x25c) == 0) return;
  {
    int v[3];
    void* w = (void*)_ZNK12WithMeshClsn13GetWallResultEv(c+0x25c);
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)w+4, v);
    int a = _ZN4cstd5atan2E5Fix12IiES1_(v[0], v[2]);
    if (_ZN5Actor14GetSubtractionEss(c, *(short*)(c+0x94), a) <= 0x4000) return;
  }
  *(unsigned char*)(c+0x108) = 0;
  func_ov020_02112110(c);
  func_0201267c(0xc5, c+0x74);
}
}
