// NONMATCHING: register allocation (div=7). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef struct { int x, y, z; } Vector3;
extern int Vec3_HorzDist(const Vector3* a, const Vector3* b);
extern void func_02012694(int a, void* p);
extern short Vec3_HorzAngle(const Vector3* a, const Vector3* b);
extern int data_ov092_02132074[];
extern int data_ov092_02132080[];
extern short SINE_TABLE[];

void func_ov092_02131878(char* c, char* a1, unsigned int a2){
  unsigned char f = *(unsigned char*)(c+0x575);
  int t = (f & 0xf) + ((f >> 4) & 3);
  if ((t & 3) != 0) return;
  if ((*(int*)(a1+8) & 3) != 2) return;
  if (Vec3_HorzDist((Vector3*)(c+0x5c), (Vector3*)(a1+0x5c)) >= 0xd2000) return;
  func_02012694(0x177, c+0x74);
  *(int*)(c+0x568) = 6;
  *(int*)(c+0x9c) = -0xc000;
  *(int*)(c+0xa0) = -0x48000;
  *(int*)(c+0xa8) = data_ov092_02132074[a2];
  *(int*)(c+0x98) = data_ov092_02132080[a2];
  if (a2 < 2) *(short*)(c+0x94) = *(short*)(a1+0x8e);
  else *(short*)(c+0x94) = Vec3_HorzAngle((Vector3*)(a1+0x5c), (Vector3*)(c+0x5c));
  {
    int n = (*(unsigned short*)(c+0x94)) >> 4;
    int* pb0 = (int*)(c+0xb0);
    *(short*)(c+0x4e0) = SINE_TABLE[2*n+1] >> 2;
    *(short*)(c+0x4e2) = 0;
    *(short*)(c+0x4e4) = (-(int)SINE_TABLE[2*n]) >> 2;
    *pb0 = *pb0 & ~0x2000000;
    *(int*)(c+0x4ec) = 0x1a9000;
  }
}
