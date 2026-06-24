typedef int Fix12;
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern void func_ov045_021118f8(char* t);
extern void func_ov045_021118c4(char* c);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
extern int _ZN21ExtendingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, Fix12, short, void*);
extern void func_020396c0(void*, int);
extern int _ZN16MeshColliderBase6EnableEP5Actor(void*, void*);
extern char data_ov045_021131d8[];
extern char data_ov045_021131d0[];
extern char data_ov045_021125b0[];

int func_ov045_02111a30(char* c) {
  void* mdl;
  void* kcl;
  mdl = _ZN5Model8LoadFileER13SharedFilePtr(data_ov045_021131d8);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd8, mdl, 1, -1);
  func_ov045_021118f8(c);
  func_ov045_021118c4(c);
  kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov045_021131d0);
  _ZN21ExtendingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c+0x158, kcl, c+0x128, 0x1000, *(short*)(c+0x8e), data_ov045_021125b0);
  func_020396c0(c+0x158, 4);
  *(unsigned char*)(c+0x1a5) = 1;
  _ZN16MeshColliderBase6EnableEP5Actor(c+0x158, c);
  *(unsigned char*)(c+0xd4) = 1;
  return 1;
}
