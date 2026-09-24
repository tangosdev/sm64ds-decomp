extern int _ZN8dActor_c18GetBitInDeathTableEv(void*);
extern void* func_ov002_020b1328(void*);
extern void _ZN10StarMarker27SpawnRedCoinStarIfNecessaryEv(void* self);
extern int _ZN8dActor_c24KillAndTrackInDeathTableEv(void*);
int func_ov002_020b10a0(char* c){
  void* marker;
  if(_ZN8dActor_c18GetBitInDeathTableEv(c)==0) return 0;
  marker = func_ov002_020b1328(c);
  if(marker) _ZN10StarMarker27SpawnRedCoinStarIfNecessaryEv(marker);
  _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
  return 1;
}
