struct Vec3 { int x, y, z; };
extern int _ZNK12WithMeshClsn13JustHitGroundEv(char* c);
extern void func_0200fc44(char* c, struct Vec3* v, int a, int z);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(char* c);
extern void func_ov102_0214beb4(char* c);
extern void func_ov102_0214be1c(char* c);

void func_ov102_0214bd90(char* r4){
  if (_ZNK12WithMeshClsn13JustHitGroundEv(r4 + 0x144)) {
    struct Vec3 v;
    v.x = *(int*)(r4 + 0x5c);
    v.y = *(int*)(r4 + 0x60);
    v.z = *(int*)(r4 + 0x64);
    func_0200fc44(r4, &v, 1, v.z);
  }
  if (_ZNK12WithMeshClsn10IsOnGroundEv(r4 + 0x144)) {
    if (*(int*)(r4 + 0x38c) == 0) {
      *(int*)(r4 + 0x98) = 0x5000;
    }
    func_ov102_0214beb4(r4);
  } else {
    func_ov102_0214be1c(r4);
  }
}
