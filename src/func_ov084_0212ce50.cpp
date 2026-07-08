//cpp
extern "C" {
void Matrix4x3_FromRotationY(void* m, short angle);
void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* a, void* sm, void* mtx, int rad, int h, unsigned int x);
struct M48 { int w[12]; };
extern M48 _ZN9Matrix3x38IDENTITYE;
void func_ov084_0212ce50(void* c){
  char* r4 = (char*)c;
  Matrix4x3_FromRotationY(r4+0x124, *(short*)(r4+0x8e));
  *(int*)(r4+0x148) = *(int*)(r4+0x5c) >> 3;
  *(int*)(r4+0x14c) = (*(int*)(r4+0x60) + 0x4000) >> 3;
  *(int*)(r4+0x150) = *(int*)(r4+0x64) >> 3;
  *(M48*)(r4+0x19c) = _ZN9Matrix3x38IDENTITYE;
  *(int*)(r4+0x1c0) = *(int*)(r4+0x5c) >> 3;
  *(int*)(r4+0x1c4) = (*(int*)(r4+0x60) - 0x8000) >> 3;
  *(int*)(r4+0x1c8) = *(int*)(r4+0x64) >> 3;
  _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(r4, r4+0x16c, r4+0x19c, 0x64000, 0x32000, 0xf);
}
}
