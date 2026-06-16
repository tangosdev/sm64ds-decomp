//cpp
extern "C" {
void Matrix4x3_FromRotationY(void* m, short angle);
void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* a, void* sm, void* mtx, int rad, int h, unsigned int x);
void func_ov019_021114ec(void* c){
  char* r4=(char*)c;
  Matrix4x3_FromRotationY(r4+0xf0, *(short*)(r4+0x8e));
  *(int*)(r4+0x114)=*(int*)(r4+0x5c)>>3;
  *(int*)(r4+0x118)=*(int*)(r4+0x60)>>3;
  *(int*)(r4+0x11c)=*(int*)(r4+0x64)>>3;
  _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(r4, r4+0x14c, r4+0xf0, 0x140000, 0x50000, 0xf);
}
}
