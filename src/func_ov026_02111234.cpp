//cpp
extern "C" {
extern void _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(void*,void*,void*,int,int,int,unsigned int);
struct M4x3 { int w[12]; };
void func_ov026_02111234(char* c){
  *(struct M4x3*)(c+0x1b0) = *(struct M4x3*)(c+0xf0);
  *(int*)(c+0x1d8) = *(int*)(c+0x1e0) >> 3;
  _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    c, c+0x188, c+0x1b0, 0x64000, 0xc8000, 0x64000, 0xf);
}
}
