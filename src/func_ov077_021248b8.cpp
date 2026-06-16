//cpp
extern "C" {
extern int _ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(void*, int);
extern int func_ov077_02124718(void*);
extern int func_ov077_02123d40(void*);
int func_ov077_021248b8(char *c){
  int v = *(int*)(c+8);
  if(_ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(c, v ? 0x1068000 : 0x7d0000)) return 1;
  func_ov077_02124718(c);
  func_ov077_02123d40(c);
  return 1;
}
}
