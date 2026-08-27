typedef int Fix12i;
extern void func_ov002_020e8c34(void* c);
extern void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void* c, void* wm, unsigned int n);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void* wm);
extern short _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(void* c, Fix12i a, Fix12i b, short ang);
extern void func_ov002_020e86ec(void* c);
extern int _ZNK10dBgCh_Actr13JustHitGroundEv(void* wm);
extern void func_ov002_020e88a8(void* c);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* wm);
extern void func_ov002_020e930c(void* c);
extern void func_ov002_020e7d08(void* c);
extern void func_ov002_020e7f2c(void* c);
void func_ov002_020e99e8(char* c){
  func_ov002_020e8c34(c);
  *(int*)(c + 0x6c) = *(int*)(c + 0x60);
  _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(c, c + 0x150, 2);
  if (_ZNK10dBgCh_Actr8IsOnWallEv(c + 0x150)){
    *(short*)(c + 0x94) = _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(c, *(int*)(c + 0xe0), *(int*)(c + 0xe8), *(short*)(c + 0x94));
  }
  func_ov002_020e86ec(c);
  if (_ZNK10dBgCh_Actr13JustHitGroundEv(c + 0x150)){
    func_ov002_020e88a8(c);
  } else if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x150)){
    *(short*)(c + 0x94) = _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(c, *(int*)(c + 0xe0), *(int*)(c + 0xe8), *(short*)(c + 0x94));
    if ((((unsigned)*(unsigned short*)(c + 0x4a2) << 0x1a) >> 0x1e) == 2)
      *(int*)(c + 0xa8) = 0xe000;
    else
      *(int*)(c + 0xa8) = 0x17000;
  }
  {
    unsigned short* ctr = (unsigned short*)(c + 0x100);
    if (*ctr == 0){
      if ((((unsigned)*(unsigned short*)(c + 0x4a2) << 0x1c) >> 0x1f) == 0)
        func_ov002_020e930c(c);
    } else {
      *ctr = *ctr - 1;
    }
  }
  func_ov002_020e7d08(c);
  func_ov002_020e7f2c(c);
}
