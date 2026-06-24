//cpp
extern "C" {
void Matrix4x3_FromRotationY(void* m, short angle);
void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* a, void* sm, void* mtx, int rad, int h, unsigned int x);
void func_ov064_02116bac(void* c){
  char* r6 = (char*)c;
  Matrix4x3_FromRotationY(r6+0x12c, *(short*)(r6+0x8e));
  *(int*)(r6+0x150) = *(int*)(r6+0x5c) >> 3;
  *(int*)(r6+0x154) = (*(int*)(r6+0x60) + *(int*)(r6+0x3ec)) >> 3;
  *(int*)(r6+0x158) = *(int*)(r6+0x64) >> 3;
  int d = *(int*)(r6+0x60) - *(int*)(r6+0x3f4);
  if(d <= 0x1000) d = 0x1000;
  int rad = (int)(((long long)d * 0x180 + 0x800) >> 12);
  int h = *(int*)(r6+0x3f0) - rad;
  if(h < 0xa000) h = 0xa000;
  Matrix4x3_FromRotationY(r6+0x3b4, *(short*)(r6+0x8e));
  *(int*)(r6+0x3d8) = *(int*)(r6+0x5c) >> 3;
  *(int*)(r6+0x3dc) = *(int*)(r6+0x60) >> 3;
  *(int*)(r6+0x3e0) = *(int*)(r6+0x64) >> 3;
  _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(r6, r6+0x370, r6+0x3b4, h, d+0x28000, 0xf);
}
}
