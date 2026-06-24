extern void MulMat4x3Mat4x3(void* out, void* a, void* b);
extern void Vec3_LslInPlace(void* v, int n);
struct M48 { int w[12]; };
extern struct M48 data_020a0e68;

void func_ov074_02121270(char* r4, char* r8, int r7){
  char* dst;
  *(int*)r4 = 0;
  *(int*)(r4 + 4) = 0;
  *(int*)(r4 + 8) = 0;
  data_020a0e68 = *(struct M48*)(r8 + 0x37c);
  dst = *(char**)(r8 + 0x224) + r7 * 0x30;
  MulMat4x3Mat4x3(dst, &data_020a0e68, &data_020a0e68);
  *(int*)r4 = *(int*)((char*)&data_020a0e68 + 0x24);
  *(int*)(r4 + 4) = *(int*)((char*)&data_020a0e68 + 0x28);
  *(int*)(r4 + 8) = *(int*)((char*)&data_020a0e68 + 0x2c);
  Vec3_LslInPlace(r4, 3);
}
