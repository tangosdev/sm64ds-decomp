struct Vec3 { int x, y, z; };
extern void Vec3_Asr(struct Vec3* d, struct Vec3* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void* m, short angY);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* self, void* sm, void* mtx, int a, int b, unsigned int g);
struct M48 { int w[12]; };
extern struct M48 data_020a0e68;

void func_ov062_0211aac0(char* r6){
  struct Vec3 v;
  Vec3_Asr(&v, (struct Vec3*)(r6 + 0x5c), 3);
  Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
  Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short*)(r6 + 0x8e));
  *(struct M48*)(r6 + 0x31c) = data_020a0e68;
  _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(r6, r6 + 0x364, r6 + 0x31c, 0xa0000, 0xa0000, 0xf);
}
