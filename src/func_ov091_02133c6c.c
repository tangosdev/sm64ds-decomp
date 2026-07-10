//cpp
typedef int s32;
typedef struct { s32 x, y, z; } Vector3;
extern "C" void Matrix4x3_FromRotationY(void* m, int angle);
extern "C" void Matrix4x3_ApplyInPlaceToRotationX(void* m, short angX);
extern "C" void MulVec3Mat4x3(const Vector3* v, const void* m, Vector3* res);
extern "C" int _ZNK12WithMeshClsn10IsOnGroundEv(char* w);
extern "C" int _ZNK12WithMeshClsn8IsOnWallEv(char* w);
extern "C" void _ZN9ActorBase18MarkForDestructionEv(char* c);
extern int data_020a0e68[];

extern "C" int func_ov091_02133c6c(char* c){
  Vector3 v;
  short* a;
  v.x = 0;
  v.y = 0;
  v.z = 0x1e000;
  a = (short*)(((int)c + 0x92) & 0xFFFFFFFFFFFFFFFFLL);
  *a = (short)(*a - 0x80);
  Matrix4x3_FromRotationY(data_020a0e68, *(short*)(c+0x94));
  Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, *(short*)(c+0x92));
  MulVec3Mat4x3(&v, data_020a0e68, (Vector3*)(c+0xa4));
  {
    unsigned short *hp = (unsigned short*)(((int)c + 0x100) & 0xFFFFFFFFFFFFFFFFLL);
    if (*hp == 0
        || _ZNK12WithMeshClsn10IsOnGroundEv(c+0x144) != 0
        || _ZNK12WithMeshClsn8IsOnWallEv(c+0x144) != 0
        || (*(int*)(c+0xb0) & 8))
      _ZN9ActorBase18MarkForDestructionEv(c);
  }
  return 1;
}
