//cpp
extern "C" {
extern void _Z14ApproachLinearRsss(short* p, short a, short b);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void* a, void* b);
extern int func_ov081_02126950(void* self, void* clsn);
extern void func_ov081_02126758(void* c);
extern void _ZN9Animation7AdvanceEv(void* p);
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern void func_ov081_021265c8(void* c);
extern void _ZN5dCc_c5ClearEv(void* p);
extern void _ZN5dCc_c6UpdateEv(void* p);
int func_ov081_02127440(char* c){
  _Z14ApproachLinearRsss((short*)(c+0x8e), *(short*)(c+0x300+0xee), 0x2bc);
  *(short*)(c+0x94) = *(short*)(c+0x8e);
  _ZN8dActor_c9UpdatePosEP5dCc_c(c, c+0x1b0);
  func_ov081_02126950(c, c+0x1e4);
  func_ov081_02126758(c);
  _ZN9Animation7AdvanceEv(c+0x124);
  if(DecIfAbove0_Byte((unsigned char*)(c+0x3f2))==0){
    func_ov081_021265c8(c);
  }
  _ZN5dCc_c5ClearEv(c+0x1b0);
  _ZN5dCc_c6UpdateEv(c+0x1b0);
  return 1;
}
}
