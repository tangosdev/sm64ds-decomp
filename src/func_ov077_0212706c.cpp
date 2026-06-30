//cpp
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern void _ZN9Animation8LoadFileER13SharedFilePtr(void*);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void*, void*, int, int, unsigned int, unsigned int);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void*, void*, void*, int, int, unsigned int, unsigned int);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void*, void*, int, int, void*, int);
extern void func_ov077_02126d5c(void*, void*);
struct V3 { int x, y, z; };
extern char data_ov077_02127c88;
extern char data_ov077_02127ca0;
extern char data_ov077_02127c90;
extern char data_ov077_02127c98;
extern struct V3 data_ov077_02127a5c;
extern char data_ov077_02127ce8;
int func_ov077_0212706c(char* c){
  struct V3 v;
  void* f;
  f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov077_02127c88);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x340, f, 1, -1);
  _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov077_02127ca0);
  _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov077_02127c90);
  _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov077_02127c98);
  *(int*)(c + 0x9c) = -0x1000;
  *(int*)(c + 0xa0) = -0x1e000;
  _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x110, c, 0x52000, 0x52000, 0x800004, 0);
  v.x = data_ov077_02127a5c.x;
  v.y = data_ov077_02127a5c.y;
  v.z = data_ov077_02127a5c.z;
  _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c + 0x144, c, &v, 0x54000, 0x32000, 0x200004, 0);
  *(short*)(c + 0x8e) = *(short*)(c + 0x94);
  _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x184, c, 0x64000, 0x64000, 0, 0);
  *(int*)(c + 0x400) = 0;
  *(int*)(c + 0x404) = *(int*)(c + 0x5c);
  *(int*)(c + 0x408) = *(int*)(c + 0x60);
  *(int*)(c + 0x40c) = *(int*)(c + 0x64);
  *(int*)(c + 0x39c) = 0x1000;
  *(int*)(c + 0x410) = *(int*)(c + 0x5c);
  *(int*)(c + 0x414) = *(int*)(c + 0x60);
  *(int*)(c + 0x418) = *(int*)(c + 0x64);
  func_ov077_02126d5c(c, &data_ov077_02127ce8);
  return 1;
}
}
