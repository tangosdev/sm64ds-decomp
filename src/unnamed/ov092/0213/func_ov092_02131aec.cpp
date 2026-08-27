//cpp
// @symbol func_ov092_02131aec
/* recovered: shared common types */
#include "common.h"
extern "C" {

extern struct Matrix4x3 data_020a0e68;
void Matrix4x3_ApplyInPlaceToRotationX(void* m, short angX);
void Matrix4x3_ApplyInPlaceToRotationZ(void* m, short angZ);
void Matrix4x3_ApplyInPlaceToRotationY(void* m, short angY);
void func_ov092_02131aec(void* c){
  char* r4 = (char*)c;
  if(*(unsigned char*)(r4+0x576) != 0){
    data_020a0e68 = *(struct Matrix4x3*)(r4+0x528);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(short*)(r4+0x8c));
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, *(short*)(r4+0x90));
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short*)(r4+0x8e));
    *(struct Matrix4x3*)(r4+0xf0) = data_020a0e68;
    *(int*)(r4+0x114) = *(int*)(r4+0x5c) >> 3;
    *(int*)(r4+0x118) = *(int*)(r4+0x60) >> 3;
    *(int*)(r4+0x11c) = *(int*)(r4+0x64) >> 3;
  } else {
    *(struct Matrix4x3*)(r4+0xf0) = *(struct Matrix4x3*)(r4+0x528);
  }
}
}
