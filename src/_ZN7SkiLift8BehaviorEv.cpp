//cpp
extern "C" {
extern void _ZN13RacingPenguin16OnPendingDestroyEv(void);
extern void _ZN9Animation7AdvanceEv(void*);
extern void _ZN12CylinderClsn5ClearEv(void*);
extern void _ZN12CylinderClsn6UpdateEv(void*);
extern int func_ov018_02111d28(void*);
}
struct Sub { virtual void m0(); virtual void m1(); virtual void m2(); virtual void m3(); };
extern "C" int _ZN7SkiLift8BehaviorEv(char* c){
  _ZN13RacingPenguin16OnPendingDestroyEv();
  _ZN9Animation7AdvanceEv(c+0x124);
  _ZN9Animation7AdvanceEv(c+0x138);
  _ZN12CylinderClsn5ClearEv(c+0x174);
  _ZN12CylinderClsn6UpdateEv(c+0x174);
  ((Sub*)(c+0xd4))->m3();
  func_ov018_02111d28(c);
  return 1;
}
