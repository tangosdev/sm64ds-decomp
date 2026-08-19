//cpp
typedef int Fix12i;
extern "C" void _ZN9Animation7AdvanceEv(char* a);
extern "C" void _ZN8dActor_c9UpdatePosEP5dCc_c(char* c, char* cl);
extern "C" void func_ov030_02111f6c(char* c, char* w);
extern "C" int _ZNK10dBgCh_Actr13JustHitGroundEv(char* w);
extern "C" void _ZN10dBgCh_Actr15ClearLimMovFlagEv(char* w);
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char* ma, void* f, int b, Fix12i c, unsigned int d);
extern "C" void func_ov030_021141a8(char* c, int v);
extern "C" void _ZN5dCc_c5ClearEv(char* cl);
extern "C" void func_ov030_02111bc4(char* c);
extern int data_ov030_02115d18[];

extern "C" int func_ov030_02113b38(char* c){
  _ZN9Animation7AdvanceEv(c+0x124);
  _ZN8dActor_c9UpdatePosEP5dCc_c(c, c+0x160);
  func_ov030_02111f6c(c, c+0x194);
  if (_ZNK10dBgCh_Actr13JustHitGroundEv(c+0x194) != 0) {
    _ZN10dBgCh_Actr15ClearLimMovFlagEv(c+0x194);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, (void*)data_ov030_02115d18[1], 0, 0x1000, 0);
    *(int*)(c+0x130) = 0x1000;
    *(int*)(c+0x38c) = *(int*)(c+0x5c);
    *(int*)(c+0x390) = *(int*)(c+0x60);
    *(int*)(c+0x394) = *(int*)(c+0x64);
    func_ov030_021141a8(c, *(int*)(c+0x3b8));
  }
  _ZN5dCc_c5ClearEv(c+0x160);
  func_ov030_02111bc4(c);
  return 1;
}
