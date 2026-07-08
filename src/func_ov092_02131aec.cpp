//cpp
extern "C" {
struct M48 { int w[12]; };
extern struct M48 MATRIX_SCRATCH_PAPER;
void Matrix4x3_ApplyInPlaceToRotationX(void* m, short angX);
void Matrix4x3_ApplyInPlaceToRotationZ(void* m, short angZ);
void Matrix4x3_ApplyInPlaceToRotationY(void* m, short angY);
void func_ov092_02131aec(void* c){
  char* r4 = (char*)c;
  if(*(unsigned char*)(r4+0x576) != 0){
    MATRIX_SCRATCH_PAPER = *(struct M48*)(r4+0x528);
    Matrix4x3_ApplyInPlaceToRotationX(&MATRIX_SCRATCH_PAPER, *(short*)(r4+0x8c));
    Matrix4x3_ApplyInPlaceToRotationZ(&MATRIX_SCRATCH_PAPER, *(short*)(r4+0x90));
    Matrix4x3_ApplyInPlaceToRotationY(&MATRIX_SCRATCH_PAPER, *(short*)(r4+0x8e));
    *(struct M48*)(r4+0xf0) = MATRIX_SCRATCH_PAPER;
    *(int*)(r4+0x114) = *(int*)(r4+0x5c) >> 3;
    *(int*)(r4+0x118) = *(int*)(r4+0x60) >> 3;
    *(int*)(r4+0x11c) = *(int*)(r4+0x64) >> 3;
  } else {
    *(struct M48*)(r4+0xf0) = *(struct M48*)(r4+0x528);
  }
}
}
