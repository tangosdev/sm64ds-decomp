//cpp
typedef int Fix12i;
typedef short s16;
extern "C" int _ZN8dActor_c13DistToCPlayerEv(char* c);
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char* ma, void* f, int b, Fix12i c, unsigned int d);
extern "C" void func_ov030_02111a00(char* c);
extern "C" s16 _ZN8dActor_c18HorzAngleToCPlayerEv(char* c);
extern "C" void _Z14ApproachLinearRsss(s16* p, s16 target, s16 step);
extern "C" void _ZN9Animation7AdvanceEv(char* a);
extern "C" void _ZN8dActor_c9UpdatePosEP5dCc_c(char* c, char* cl);
extern "C" void func_ov030_02111dd0(char* c);
extern "C" void func_ov030_02111f6c(char* c, char* w);
extern "C" void func_ov030_02111bc4(char* c);
extern "C" void func_ov030_02111ea4(char* c);
extern "C" void _ZN5dCc_c5ClearEv(char* cl);
extern "C" void _ZN5dCc_c6UpdateEv(char* cl);
extern "C" void func_ov030_02111890(char* c);
extern int data_ov030_02115d18[];

extern "C" int func_ov030_02113ff0(char* c){
  int dist = _ZN8dActor_c13DistToCPlayerEv(c);
  int b;
  if (dist > 0x15e000) {
    *(int*)(c+0x98) = 0xc000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, (void*)data_ov030_02115d18[1], 0, 0x1000, 0);
  } else if (dist <= 0xfa000) {
    *(int*)(c+0x98) = 0;
    func_ov030_02111a00(c);
  }
  _Z14ApproachLinearRsss((s16*)(c+0x8e), _ZN8dActor_c18HorzAngleToCPlayerEv(c), 0x514);
  *(s16*)(c+0x94) = *(s16*)(c+0x8e);
  *(int*)(c+0x130) = 0x1000;
  _ZN9Animation7AdvanceEv(c+0x124);
  b = (*(unsigned short*)(c+0xc) == 0x10b);
  if (b) {
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c+0x160);
    func_ov030_02111dd0(c);
    func_ov030_02111f6c(c, c+0x194);
    func_ov030_02111bc4(c);
  } else {
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c+0x160);
    func_ov030_02111f6c(c, c+0x194);
    func_ov030_02111bc4(c);
    func_ov030_02111ea4(c);
  }
  _ZN5dCc_c5ClearEv(c+0x160);
  _ZN5dCc_c6UpdateEv(c+0x160);
  func_ov030_02111890(c);
  return 1;
}
