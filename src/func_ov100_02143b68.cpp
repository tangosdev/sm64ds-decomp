//cpp
typedef int Fix12;
typedef short s16;
extern "C" {
void Matrix4x3_FromRotationXYZExt(void* m, int x, int y, int z);
void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* self, void* sm, void* mtx, Fix12 a, Fix12 b, unsigned int g);
}
struct Mtx { int w[12]; };
extern Mtx _ZN9Matrix3x38IDENTITYE;

extern "C" void func_ov100_02143b68(char* c) {
  Mtx tmp;
  int i;
  char* mdst;
  char* rd;
  char* st;
  char* sm;
  Matrix4x3_FromRotationXYZExt(c+0x328, *(s16*)(c+0x8c), *(s16*)(c+0x8e), *(s16*)(c+0x90));
  *(int*)(c+0x34c) = *(int*)(c+0x5c) >> 3;
  *(int*)(c+0x350) = *(int*)(c+0x60) >> 3;
  *(int*)(c+0x354) = *(int*)(c+0x64) >> 3;
  _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c+0x640, c+0x328, 0x15e000, 0x1f4000, 0xf);
  tmp = _ZN9Matrix3x38IDENTITYE;
  i = 0;
  mdst = c + 0x370;
  rd = c;
  st = c;
  sm = c + 0x550;
  for (; i < 6; i++) {
    *(Mtx*)(mdst+0x1c) = tmp;
    *(int*)(st+0x3b0) = *(int*)(rd+0x6d8) >> 3;
    *(int*)(st+0x3b4) = *(int*)(rd+0x6dc) >> 3;
    *(int*)(st+0x3b8) = *(int*)(rd+0x6e0) >> 3;
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, sm, mdst+0x1c, 0x78000, 0x1f4000, 0xf);
    mdst += 0x50;
    rd += 0xc;
    st += 0x50;
    sm += 0x28;
  }
}
