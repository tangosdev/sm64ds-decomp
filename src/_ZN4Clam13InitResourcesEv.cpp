//cpp
typedef int Fix12i;
extern "C" void _ZN9Animation8LoadFileER13SharedFilePtr(void* sfp);
extern "C" void* _ZN5Model8LoadFileER13SharedFilePtr(void* sfp);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(char* mb, void* f, int a, int b);
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char* ma, void* f, int b, Fix12i c, unsigned int d);
extern "C" void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(char* mc, char* a, Fix12i r, Fix12i h, unsigned int f1, unsigned int f2);
extern "C" void func_ov064_0211a9b4(char* t);
extern int data_ov064_0211c9cc[];
extern int data_ov064_0211c9bc[];
extern int data_ov064_0211c9c4[];

extern "C" int _ZN4Clam13InitResourcesEv(char* c){
  _ZN9Animation8LoadFileER13SharedFilePtr(data_ov064_0211c9cc);
  _ZN9Animation8LoadFileER13SharedFilePtr(data_ov064_0211c9bc);
  void* m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov064_0211c9c4);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, m, 1, -1);
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, (void*)data_ov064_0211c9bc[1], 0x40000000, 0x1000, 0);
  _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c+0x138, c, 0x64000, 0x64000, 0x200004, 0);
  func_ov064_0211a9b4(c);
  *(unsigned char*)(c+0x16c) = 0;
  return 1;
}
