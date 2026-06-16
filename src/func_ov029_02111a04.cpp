//cpp
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* bmd, int a, int b);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* self, void* actor, int fix, int t, unsigned int u, unsigned int v);
extern int data_ov029_02114270[];
int func_ov029_02111a04(char* c){
  void* m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov029_02114270);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, m, 1, -1);
  _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c+0x124, c, 0x32000, 0x64000, 0x800002, 0);
  *(int*)(c+0x158) = 0;
  *(char*)(c+0x15c) = *(int*)(c+8) & 1;
  *(char*)(c+0x15d) = 0;
  *(short*)(c+0x8e) = 0;
  return 1;
}
}
