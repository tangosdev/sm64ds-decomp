// NONMATCHING: different op / idiom (div=29). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern unsigned char CURRENT_GAMEMODE;
extern int NEXT_ACTOR_UPDATE_FLAGS;
extern int _ZN9Animation8FinishedEv(char* a);
extern void func_ov002_020e7e58(char* c);
extern void _ZN5Actor11UntrackStarERa(char* c, signed char* p);
extern void _ZN9ActorBase18MarkForDestructionEv(char* c);
extern void _ZN5Actor24KillAndTrackInDeathTableEv(char* c);
extern void _ZN5Event8ClearBitEj(unsigned int b);
void func_ov002_020e8618(char* c){
  if(_ZN9Animation8FinishedEv(c+0x35c) == 0) return;
  func_ov002_020e7e58(c);
  *(unsigned short*)(c+0x4a2) &= ~2;
  _ZN5Actor11UntrackStarERa(c, (signed char*)(c+0x498));
  if((int)(CURRENT_GAMEMODE == 1) != 0){
    _ZN9ActorBase18MarkForDestructionEv(c);
  }else{
    _ZN5Actor24KillAndTrackInDeathTableEv(c);
  }
  *(int*)(*(char**)(c+0x438)+0xb0) &= ~0x4000000;
  *(int*)(c+0xb0) &= ~0x4000000;
  NEXT_ACTOR_UPDATE_FLAGS &= ~0x4000000;
  _ZN5Event8ClearBitEj(0x1e);
  _ZN5Event8ClearBitEj(0x1d);
}
