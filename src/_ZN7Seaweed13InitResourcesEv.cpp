//cpp
extern "C" {
typedef int Fix12i;
struct SharedFilePtr; struct BMD_File; struct BCA_File;
extern SharedFilePtr data_ov002_0210e104;
extern SharedFilePtr data_ov002_0210e0fc;
extern BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr& f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* m, BMD_File* f, int a, int b);
extern BCA_File* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr& f);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* m, BCA_File* f, int a, Fix12i b, unsigned int c);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* m, void* a, Fix12i r, Fix12i h, unsigned int f1, unsigned int f2);
extern void func_ov002_020bc664(void* t);
int _ZN7Seaweed13InitResourcesEv(char* c){
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210e104), 1, -1);
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, _ZN9Animation8LoadFileER13SharedFilePtr(data_ov002_0210e0fc), 0, 0x1000, 0);
  _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c+0x138, c, 0x3c000, 0x78000, 0x100002, 0x8000);
  *(int*)(c+0x16c) = 0x1000;
  func_ov002_020bc664(c);
  return 1;
}
}
