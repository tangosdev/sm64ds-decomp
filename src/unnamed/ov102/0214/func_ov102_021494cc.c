extern void func_ov102_02149684(int* dst, int* src);
extern int func_ov102_02149100(void* a, int* s, int b, int d, int e);
extern int _ZN8dActor_c24KillAndTrackInDeathTableEv(void*);
int func_ov102_021494cc(char* c){
  int s[3];
  func_ov102_02149684(s, (int*)c);
  int r2 = (*(unsigned int*)(c+8) >> 8) & 0xff;
  if(r2 == 0xff) r2 = 1;
  int w[3];
  w[0] = s[0]; w[1] = s[1]; w[2] = s[2];
  func_ov102_02149100(c, w, r2, 0x1800, 0);
  return _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
}
