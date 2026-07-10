//cpp
// NONMATCHING: different op / idiom (div=44). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct Cls { virtual void dummy(); };
typedef void (Cls::*PMF)();
extern PMF data_ov084_02130e80[];
extern "C" {
extern int _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(void*, void*, void*, unsigned int);
extern void _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(void*, void*);
extern void _ZN9Animation7AdvanceEv(void*);
extern void func_ov084_0212f204(void*);
extern void func_ov084_0212ec60(void*);
extern void _ZN12CylinderClsn5ClearEv(void*);
extern void _ZN12CylinderClsn6UpdateEv(void*);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void*, void*);
int _ZN12PiranhaPlant8BehaviorEv(char* c){
  int r;
  int old;
  r = _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(c, c + 0x1c4, c + 0x110, 1);
  if (r != 0) {
    if (r == 2) {
      *(unsigned char*)(c + 0x108) = 0;
      *(int*)(c + 0x458) = 7;
      *(int*)(c + 0x80) = 0;
      *(int*)(c + 0x84) = 0;
      *(int*)(c + 0x88) = 0;
      *(int*)(c + 0x5c) = *(int*)(c + 0x44c);
      *(int*)(c + 0x60) = *(int*)(c + 0x450);
      *(int*)(c + 0x64) = *(int*)(c + 0x454);
    }
    return 1;
  }
  {
    _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(c, c + 0x380);
    _ZN9Animation7AdvanceEv(c + 0x160);
    func_ov084_0212f204(c);
    old = *(int*)(c + 0x458);
    (((Cls*)c)->*data_ov084_02130e80[old])();
    *(unsigned short*)(c + 0x100) = *(unsigned short*)(c + 0x100) + 1;
    if (old != *(int*)(c + 0x458)) {
      if (*(int*)(c + 0x458) == 5) {
        int* pb0 = (int*)(c + 0xb0);
        *pb0 = *pb0 & ~0x10000000;
      }
      *(unsigned short*)(c + 0x100) = 0;
      *(int*)(c + 0x478) = 0;
    }
    func_ov084_0212ec60(c);
    _ZN12CylinderClsn5ClearEv(c + 0x380);
    _ZN12CylinderClsn5ClearEv(c + 0x3b4);
    _ZN12CylinderClsn5ClearEv(c + 0x3e8);
    if (*(unsigned char*)(c + 0x45c) != 0) {
      _ZN12CylinderClsn6UpdateEv(c + 0x380);
      _ZN12CylinderClsn6UpdateEv(c + 0x3b4);
      if (*(int*)(c + 0x458) == 2) {
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c + 0x3e8, c + 0x440);
      }
      _ZN12CylinderClsn6UpdateEv(c + 0x3e8);
    }
  }
  return 1;
}
}
