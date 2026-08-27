//cpp
extern "C" {
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void*);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void*);
extern void _ZN12dEnemyBase_c9SpawnCoinEv(void*);
extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(void*);
int func_ov002_020ae454(char* c, void* a){
  if(*(unsigned short*)(c+0x102)==0 || _ZNK10dBgCh_Actr10IsOnGroundEv(a)!=0 || _ZNK10dBgCh_Actr8IsOnWallEv(a)!=0){
    _ZN12dEnemyBase_c9SpawnCoinEv(c);
    _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
    *(int*)(c+0x10c)=0;
    return 1;
  }
  return 0;
}
}
