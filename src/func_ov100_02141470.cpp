//cpp
// NONMATCHING: register allocation (div=45). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct V3 { int x, y, z; };
extern short SINE_TABLE[];
extern "C" short Vec3_HorzAngle(struct V3 *a, struct V3 *b);
extern "C" void _Z14ApproachLinearRsss(short *p, short target, short step);
extern "C" short Vec3_VertAngle(struct V3 *a, struct V3 *b);
extern "C" void _ZN5Actor9UpdatePosEP12CylinderClsn(void *thiz, void *cyl);
extern "C" int _ZN5Actor15IsPlayerInRangeEi(void *thiz, int r);

extern "C" void func_ov100_02141470(char *c){
  _Z14ApproachLinearRsss((short*)(c+0x94), Vec3_HorzAngle((struct V3*)(c+0x5c), (struct V3*)(c+0x3d4)), 0x800);
  _Z14ApproachLinearRsss((short*)(c+0x92), Vec3_VertAngle((struct V3*)(c+0x5c), (struct V3*)(c+0x3d4)), 0x50);
  _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
  *(int*)(c+0x60) = *(int*)(c+0x60)
    - ((int)(((long long)*(int*)(c+0x98) * SINE_TABLE[((unsigned short)*(short*)(c+0x92) >> 4) * 2] + 0x800) >> 12)
       + (SINE_TABLE[((unsigned short)(short)(((*(int*)(c+0x3e8)) << 16) / 100) >> 4) * 2 + 1] * 0x14) / 4);
  (*(int*)(c+0x3e8))++;
  if(*(int*)(c+0x3e8) > 0x64) *(int*)(c+0x3e8) = 0;
  if(_ZN5Actor15IsPlayerInRangeEi(c, 0xbb8)) return;
  *(int*)(c+0x5c) = *(int*)(c+0x3d4);
  *(int*)(c+0x60) = *(int*)(c+0x3d8);
  *(int*)(c+0x64) = *(int*)(c+0x3dc);
  *(int*)(c+0x3e4) = 1;
}
