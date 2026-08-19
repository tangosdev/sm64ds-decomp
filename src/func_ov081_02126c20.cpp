//cpp
extern "C" {
extern void _ZN9Animation7AdvanceEv(void* a);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void* a, void* cyl);
extern void func_02038414(void* a);
extern int _ZNK10dBgCh_Actr13JustHitGroundEv(void* a);
extern int DecIfAbove0_Byte(void* p);
extern void func_ov081_02126700(char* c);
int func_ov081_02126c20(char* c){
  *(short*)(c+0x8c) = *(short*)(c+0x8c) - 0x1000;
  _ZN9Animation7AdvanceEv(c+0x124);
  _ZN8dActor_c9UpdatePosEP5dCc_c(c, c+0x1b0);
  func_02038414(c+0x1e4);
  if(_ZNK10dBgCh_Actr13JustHitGroundEv(c+0x1e4)!=0 || DecIfAbove0_Byte(c+0x3f2)==0){
    func_ov081_02126700(c);
  }
  return 1;
}
}
