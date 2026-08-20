typedef int Fix12;
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern int _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void*);
extern int _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
extern void* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void*);
extern int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, Fix12, short, void*);
int func_ov002_020b6584(void* c, void** f, int a) {
  void* mdl;
  void* kcl;
  mdl = _ZN5Model8LoadFileER13SharedFilePtr(f[0]);
  _ZN9ModelBase7SetFileEP8BMD_Fileii((char*)c+0xd4, mdl, 1, -1);
  _ZN10dBgActor_c21UpdateModelPosAndRotYEv(c);
  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
  kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(f[1]);
  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block((char*)c+0x124, kcl, (char*)c+0x2ec, 0x1000, *(short*)((char*)c+0x8e), f[2]);
  *(int*)((char*)c+0x320) = *(int*)((char*)c+0x60);
  *(int*)((char*)c+0x324) = a;
  return 1;
}
