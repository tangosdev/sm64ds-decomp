//cpp
struct V3 { int x,y,z; V3(){} V3(const V3&o){x=o.x;y=o.y;z=o.z;} };
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern void _ZN11ShadowModel10InitCuboidEv(void*);
extern void _ZN7PathPtr6FromIDEj(void*, unsigned int);
extern int _ZNK7PathPtr8NumNodesEv(void*);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void*, void*, V3*, int, int, unsigned int, unsigned int);
extern int func_ov053_021112a4(char*);
extern int data_ov026_02113ea0[];
extern int SAVE_DATA[];
extern V3 data_ov032_02113a9c;
int func_ov026_02111598(char *c){
  void *f = _ZN5Model8LoadFileER13SharedFilePtr((void*)data_ov026_02113ea0);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, f, 1, -1);
  if((SAVE_DATA[1] & 0x204) == 0) return 0;
  _ZN11ShadowModel10InitCuboidEv(c+0x188);
  _ZN7PathPtr6FromIDEj(c+0x164, *(unsigned int*)(c+8) & 0xff);
  *(int*)(c+0x16c) = _ZNK7PathPtr8NumNodesEv(c+0x164);
  *(int*)(c+0x180) = 1;
  V3 g = data_ov032_02113a9c;
  _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c+0x124, c, &g, 0x50000, 0x12c000, 0x80000c, 0);
  func_ov053_021112a4(c);
  return 1;
}
}
