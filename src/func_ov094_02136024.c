//cpp
// NONMATCHING: different op / idiom (div=35). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
typedef struct { int x, y, z; } Vec3;
typedef struct PMF PMF;
char* _ZN5Actor13ClosestPlayerEv(void);
void Vec3_Sub(Vec3* out, Vec3* a, Vec3* b);
int LenVec3(Vec3* v);
void Matrix4x3_FromRotationY(void* m, int angle);
void MulVec3Mat4x3(Vec3* in, void* m, Vec3* out);
short Vec3_HorzAngle(const Vec3* a, const Vec3* b);
int func_ov094_02136188(void* c, PMF* p);
extern char MATRIX_SCRATCH_PAPER[];
extern PMF data_ov094_02136b60;

int func_ov094_02136024(char* c){
  char* p = _ZN5Actor13ClosestPlayerEv();
  if (p != 0 && *(int*)(p+0x37c) != 0) {
    char* ip = p + 0x5c;
    Vec3 pp;
    pp.x = *(int*)ip;
    pp.y = *(int*)(ip+4);
    pp.z = *(int*)(ip+8);
    *(int*)(c+0x60) = *(int*)(p+0x644) + 0x190000;
    *(int*)(c+0x3d0) = (int)p;
    Vec3 d;
    Vec3_Sub(&d, (Vec3*)(c+0x5c), &pp);
    if (LenVec3(&d) < 0x28000) {
      d.x = *(int*)(c+0x5c);
      d.y = *(int*)(c+0x60);
      d.z = *(int*)(c+0x64);
      d.x = 0;
      d.y = 0;
      d.z = 0x320000;
      Matrix4x3_FromRotationY(MATRIX_SCRATCH_PAPER, 0x4000);
      MulVec3Mat4x3(&d, MATRIX_SCRATCH_PAPER, (Vec3*)(c+0x3d8));
      *(int*)(c+0x3d8) += *(int*)(c+0x5c);
      *(int*)(c+0x3dc) += *(int*)(c+0x60);
      *(int*)(c+0x3e0) += *(int*)(c+0x64);
      *(short*)(c+0x94) = Vec3_HorzAngle((Vec3*)(c+0x5c), (Vec3*)(c+0x3d8));
      *(short*)(c+0x8e) = *(short*)(c+0x94);
      func_ov094_02136188(c, &data_ov094_02136b60);
    }
  }
  return 1;
}
}
