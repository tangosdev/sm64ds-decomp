//cpp
extern "C" {
void Vec3_Asr(void* d, void* s, int sh);
int _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(void*, void*, void*, int, void*);
int func_ov072_0211fcb0(void*, int);
extern int data_0209f43c[];
extern int data_0209b3ec[];
int func_ov072_0211f48c(char* c){
  int b = (int)((*(int*)(c+0xb0) & 8) != 0);
  if(b == 0) return 1;
  int sp4[3];
  int v[3];
  Vec3_Asr(v, c+0x33c, 3);
  if(_ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(data_0209f43c, data_0209b3ec, v, 0x1f400, sp4) < 0x1194000) return 1;
  *(int*)(c+0x5c) = *(int*)(c+0x33c);
  *(int*)(c+0x60) = *(int*)(c+0x340);
  *(int*)(c+0x64) = *(int*)(c+0x344);
  *(short*)(c+0x8c) = *(short*)(c+0x348);
  *(short*)(c+0x8e) = *(short*)(c+0x34a);
  *(short*)(c+0x90) = *(short*)(c+0x34c);
  *(short*)(c+0x92) = *(short*)(c+0x8c);
  *(short*)(c+0x94) = *(short*)(c+0x8e);
  *(short*)(c+0x96) = *(short*)(c+0x90);
  *(int*)(c+0x390) = 0;
  func_ov072_0211fcb0(c, 0);
  return 1;
}
}
