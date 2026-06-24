//cpp
typedef int Fix12i;
extern "C" void* _ZN5Model8LoadFileER13SharedFilePtr(void* sfp);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(char* mb, void* f, int a, int b);
extern "C" void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(char* mc, char* a, Fix12i r, Fix12i h, unsigned int f1, unsigned int f2);
extern "C" void _ZN8Platform21UpdateModelPosAndRotYEv(char* c);
extern "C" void _ZN8Platform19UpdateClsnPosAndRotEv(char* c);
extern "C" void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* sfp);
extern "C" void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(char* mmc, void* kcl, char* mtx, Fix12i s, short sh, void* clps);
extern int data_ov081_02128fd8[];
extern int data_ov081_02128fd0[];
extern int data_ov002_0210d8d4[];

extern "C" int func_ov081_02127ff0(char* c){
  void* m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov081_02128fd8);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, m, 1, 0x17);
  _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c+0x320, c, 0xc8000, 0x12c000, 0x800002, 0x44000);
  _ZN8Platform21UpdateModelPosAndRotYEv(c);
  _ZN8Platform19UpdateClsnPosAndRotEv(c);
  void* k = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov081_02128fd0);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c+0x124, k, c+0x2ec, 0x1000, *(short*)(c+0x8e), data_ov002_0210d8d4);
  *(int*)(c+0x358) = 0x1000;
  return 1;
}
