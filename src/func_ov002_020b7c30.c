extern int _ZN8dActor_c9UpdatePosEP5dCc_c(void*, void*);
extern int _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void*, void*, unsigned int);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void*);
extern int func_ov002_020b6fcc(void*);
extern int _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(void*, int, int, int, int);
extern int _ZN8SaveData13PlayerLoseCapEv(void);
extern int _ZN7fBase_c18MarkForDestructionEv(void*);
extern int data_02092138;

int func_ov002_020b7c30(void* c) {
  if (*(int*)((char*)c + 0x9c) != 0) {
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, (char*)c + 0x110);
    _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(c, (char*)c + 0x144, 0);
    *(unsigned*)((char*)c + 0x12c) |= 0x8000;
    if (_ZNK10dBgCh_Actr10IsOnGroundEv((char*)c + 0x144)) {
      *(int*)((char*)c + 0x98) = 0;
      func_ov002_020b6fcc(c);
      _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(c, 0x32000, 0x32000, 0x1000000, 0x1000000);
    }
  }
  if (data_02092138 > *(int*)((char*)c + 0x60)) {
    _ZN8SaveData13PlayerLoseCapEv();
    _ZN7fBase_c18MarkForDestructionEv(c);
  }
  return 1;
}
