//cpp
// NONMATCHING: different op / idiom (div=23). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef int s32;
typedef struct { s32 x, y, z; } Vector3;
extern "C" void Matrix4x3_FromRotationY(void* m, int angle);
extern "C" void Matrix4x3_ApplyInPlaceToRotationX(void* m, short angX);
extern "C" void MulVec3Mat4x3(const Vector3* v, const void* m, Vector3* res);
extern "C" int _ZNK12WithMeshClsn10IsOnGroundEv(char* w);
extern "C" int _ZNK12WithMeshClsn8IsOnWallEv(char* w);
extern "C" void _ZN9ActorBase18MarkForDestructionEv(char* c);
extern int MATRIX_SCRATCH_PAPER[];

extern "C" int func_ov091_02133c6c(char* c){
  Vector3 v;
  v.z = 0x1e000;
  short* a = (short*)(c+0x92);
  v.x = 0;
  v.y = 0;
  *a = *a - 0x80;
  Matrix4x3_FromRotationY(MATRIX_SCRATCH_PAPER, *(short*)(c+0x94));
  Matrix4x3_ApplyInPlaceToRotationX(MATRIX_SCRATCH_PAPER, *(short*)(c+0x92));
  MulVec3Mat4x3(&v, MATRIX_SCRATCH_PAPER, (Vector3*)(c+0xa4));
  if (*(unsigned short*)(c+0x100) != 0
      && _ZNK12WithMeshClsn10IsOnGroundEv(c+0x144) == 0
      && _ZNK12WithMeshClsn8IsOnWallEv(c+0x144) == 0
      && (*(int*)(c+0xb0) & 8))
    _ZN9ActorBase18MarkForDestructionEv(c);
  return 1;
}
