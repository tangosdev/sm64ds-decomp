//cpp
extern "C" {
void Matrix4x3_FromRotationY(void* m, short angle);
void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* a, void* sm, void* mtx, int rad, int h, unsigned int x);

struct M48 { int w[12]; };
extern M48 _ZN9Matrix3x38IDENTITYE;
extern char data_ov089_021328b4[];
extern int data_ov002_02110964;

void func_ov089_02131f54(void* c){
  char* r4=(char*)c;
  Matrix4x3_FromRotationY(r4+0x130, *(short*)(r4+0x8e));
  *(int*)(r4+0x154)=*(int*)(r4+0x5c)>>3;
  *(int*)(r4+0x158)=*(int*)(r4+0x60)>>3;
  *(int*)(r4+0x15c)=*(int*)(r4+0x64)>>3;
  if (*(int*)(data_ov089_021328b4 + (*(int*)(r4+0x444)<<2)) != 0 && *(int*)(r4+0x448)==0) {
    Matrix4x3_FromRotationY(r4+0x194, *(short*)(r4+0x8e));
    *(int*)(r4+0x1b8)=*(int*)(r4+0x5c)>>3;
    *(int*)(r4+0x1bc)=(*(int*)(r4+0x60)+0x64000)>>3;
    *(int*)(r4+0x1c0)=*(int*)(r4+0x64)>>3;
  }
  *(M48*)(r4+0x1f0)=_ZN9Matrix3x38IDENTITYE;
  *(int*)(r4+0x214)=*(int*)(r4+0x5c)>>3;
  *(int*)(r4+0x218)=*(int*)(r4+0x60)>>3;
  *(int*)(r4+0x21c)=*(int*)(r4+0x64)>>3;
  if (*(int*)(r4+0x174) == (&data_ov002_02110964)[1]) {
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(r4, r4+0x1c8, r4+0x1f0, 0x96000, 0x3e8000, 0xf);
  }
}
}
