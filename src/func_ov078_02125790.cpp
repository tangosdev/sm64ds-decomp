//cpp
// NONMATCHING: different op / idiom (div=42). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct Mtx43 { int m[12]; };
struct Vec3 { int x, y, z; };
extern int func_ov078_02123804(char* c);
extern int _ZN5Actor18HorzAngleToCPlayerEv(void* a);
extern void ApproachAngle(short* p, int target, int step, int max);
extern int _ZNK9Animation12WillHitFrameEi(void* a, int f);
extern void func_ov078_02125c24(char* c, int v);
extern void func_02012694(int a, void* b);
extern void MulMat4x3Mat4x3(void* d, Mtx43* a, Mtx43* b);
extern void Vec3_Lsl(Vec3* d, Vec3* s, int sh);
extern int _ZN5Actor17HugeLandingDustAtER7Vector3b(void* a, Vec3& v, bool b);
extern int _ZN9Animation8FinishedEv(void* a);
extern void func_ov078_02125c48(char* c, void* p);
extern Mtx43 MATRIX_SCRATCH_PAPER;
extern int data_ov078_0212703c;

extern "C" int func_ov078_02125790(char* self)
{
  Vec3 s;
  Vec3 d;
  if (func_ov078_02123804(self) == 1) return 1;
  ApproachAngle((short*)(self + 0x94), _ZN5Actor18HorzAngleToCPlayerEv(self), 1, 0x500);
  *(short*)(self + 0x8e) = *(short*)(self + 0x94);
  if (_ZNK9Animation12WillHitFrameEi(self + 0x31c, 0x46)) {
    func_ov078_02125c24(self, 0x7d0000);
    func_02012694(0x12c, self + 0x74);
    s.x = 0;
    s.y = 0;
    s.z = 0;
    MATRIX_SCRATCH_PAPER = *(Mtx43*)(self + 0x2e8);
    MulMat4x3Mat4x3((char*)*(void**)(self + 0x2e0) + 0x120, &MATRIX_SCRATCH_PAPER, &MATRIX_SCRATCH_PAPER);
    s.x = MATRIX_SCRATCH_PAPER.m[9];
    s.y = MATRIX_SCRATCH_PAPER.m[10];
    s.z = MATRIX_SCRATCH_PAPER.m[11];
    Vec3_Lsl(&d, &s, 3);
    {
      Vec3 v;
      v.x = d.x;
      v.y = d.y;
      v.z = d.z;
      s.x = d.x;
      s.y = d.y;
      s.z = d.z;
      _ZN5Actor17HugeLandingDustAtER7Vector3b(self, v, 1);
    }
  }
  if (_ZN9Animation8FinishedEv(self + 0x31c)) {
    func_ov078_02125c48(self, &data_ov078_0212703c);
  }
  return 1;
}
