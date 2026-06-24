typedef int Fix12;
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern void func_ov016_02112a9c(char* t);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, Fix12, short, void*);
extern int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int, unsigned int, void*, void*, int, int);
extern char data_ov016_02114e24[];
extern char data_ov016_02114e1c[];
extern char data_ov016_02113cac[];

int func_ov016_02112e1c(char* c) {
  void* mdl;
  void* kcl;
  mdl = _ZN5Model8LoadFileER13SharedFilePtr(data_ov016_02114e24);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, mdl, 1, -1);
  func_ov016_02112a9c(c);
  _ZN8Platform19UpdateClsnPosAndRotEv(c);
  kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov016_02114e1c);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c+0x124, kcl, c+0x2ec, 0x199, *(short*)(c+0x8e), data_ov016_02113cac);
  _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x3b, 0, c+0x5c, 0, *(signed char*)(c+0xcc), -1);
  return 1;
}
