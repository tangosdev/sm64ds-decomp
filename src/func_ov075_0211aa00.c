extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void* m, short angY);
extern void InvMat4x3(void* d, void* s);
struct M48 { int w[12]; };
extern struct M48 data_020a0e68;

void func_ov075_0211aa00(char* r4){
  Matrix4x3_FromTranslation(&data_020a0e68,
    (*(int*)(r4) - 0xfa000) >> 3,
    (*(int*)(r4 + 4) - 0xfa000) >> 3,
    *(int*)(r4 + 8) >> 3);
  Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short*)(r4 + 0xe));
  *(struct M48*)(r4 + 0x14) = data_020a0e68;
  InvMat4x3(&data_020a0e68, &data_020a0e68);
  *(struct M48*)(r4 + 0x44) = data_020a0e68;
}
