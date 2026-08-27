extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void*);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void*);
extern int _ZN8dActor_c13SmallPoofDustEv(void*);
extern int func_02012694(int, void*);
extern int _ZN7fBase_c18MarkForDestructionEv(void*);
extern int func_ov002_020feb50(void*);
int func_ov002_020fec94(void* c) {
  if (*(unsigned short*)((char*)c+0x100)==0
      || _ZNK10dBgCh_Actr10IsOnGroundEv((char*)c+0x144)
      || _ZNK10dBgCh_Actr8IsOnWallEv((char*)c+0x144)) {
    _ZN8dActor_c13SmallPoofDustEv(c);
    func_02012694(0x166, (char*)c+0x74);
    _ZN7fBase_c18MarkForDestructionEv(c);
    return 1;
  }
  if (*(int*)((char*)c+0x354)==0) func_ov002_020feb50(c);
  return 1;
}
