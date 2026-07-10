//cpp
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* bmd, int a, int b);
extern void _ZN11ShadowModel10InitCuboidEv(void* self);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void* self);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* self);
extern void func_ov098_02137c8c(char* t);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* self, void* kcl, void* mtx, int fix, short s, void* clps);

struct Entry { void* a; void* b; void* c; };
extern struct Entry data_ov098_0213c380[];

int _ZN14ArrowSignRight13InitResourcesEv(char* c){
  unsigned short field = *(unsigned short*)(c+0xc);
  if(field != 0x12b){
    if(field == 0x12c)
      *(unsigned char*)(c+0x37c) = 1;
  } else {
    *(unsigned char*)(c+0x37c) = 0;
  }
  unsigned int idx = *(unsigned char*)(c+0x37c);
  void* m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov098_0213c380[idx].a);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, m, 1, -1);
  _ZN11ShadowModel10InitCuboidEv(c+0x320);
  _ZN8Platform21UpdateModelPosAndRotYEv(c);
  _ZN8Platform19UpdateClsnPosAndRotEv(c);
  func_ov098_02137c8c(c);
  unsigned int idx2 = *(unsigned char*)(c+0x37c);
  void* kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov098_0213c380[idx2].b);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
      c+0x124, kcl, c+0x2ec, 0x199, *(short*)(c+0x8e), data_ov098_0213c380[idx2].c);
  return 1;
}
}
