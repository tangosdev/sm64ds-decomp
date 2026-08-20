extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void*);
extern int _ZN12dEnemyBase_c9SpawnCoinEv(void*);
extern int _ZN8dActor_c24KillAndTrackInDeathTableEv(void*);
int func_ov002_020ae608(void* c, void* a){
  if(_ZNK10dBgCh_Actr10IsOnGroundEv(a)==0) return 0;
  _ZN12dEnemyBase_c9SpawnCoinEv(c);
  _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
  *(int*)((char*)c+0x10c)=0;
  return 1;
}
