//cpp
extern "C" {
extern char* _ZN5Actor22ClosestNonVanishPlayerEv(void);
extern int Vec3_HorzDist(void* a, void* b);
extern short Vec3_HorzAngle(void* a, void* b);
extern void _Z14ApproachLinearRsss(void* a, short b, short c);
extern int func_ov077_021238bc(int unused, int x);
extern void _Z14ApproachLinearRiii(void* a, int b, int c);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* a, void* b);
extern int _ZN9Spindrift8BehaviorEv(void* a, void* b);
void func_ov077_0212390c(char* c){
  char* p = _ZN5Actor22ClosestNonVanishPlayerEv();
  char* tgt;
  int r6;
  if(p != 0){
    r6 = (*(int*)(c+8) != 0) ? 0x1068000 : 0x7d0000;
    if(Vec3_HorzDist(c+0x3f8, p+0x5c) < r6){
      int d = *(int*)(c+0x3fc) - *(int*)(p+0x60);
      if(d < 0) d = -d;
      if(d < 0x5dc000){ tgt = p+0x5c; goto L64; }
    }
    tgt = c+0x3f8;
  } else {
    tgt = c+0x3f8;
  }
L64:
  {
    int hd = Vec3_HorzDist(c+0x5c, tgt);
    short ha = Vec3_HorzAngle(c+0x5c, tgt);
    _Z14ApproachLinearRsss(c+0x8e, ha, 0x5e8);
    if(*(unsigned char*)(c+0x41d) != 0){
      *(short*)(c+0x94) = *(short*)(c+0x8e) - func_ov077_021238bc((int)c, hd);
    } else {
      *(short*)(c+0x94) = *(short*)(c+0x8e) + func_ov077_021238bc((int)c, hd);
    }
    _Z14ApproachLinearRiii(c+0x60, *(int*)(tgt+4) + 0x12c000, 0x2000);
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c+0x1c4);
    _ZN9Spindrift8BehaviorEv(c, c+0x204);
  }
}
}
