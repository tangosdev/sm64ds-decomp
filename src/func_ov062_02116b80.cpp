//cpp
extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);
extern unsigned int RandomIntInternal(void* s);
extern int data_ov062_0211ddf8[];
extern int RNG_STATE[];
int func_ov062_02116b80(char* c){
  *(short*)(c+0x300+0xf4) = (short)((RandomIntInternal(RNG_STATE) >> 8) << 0xc);
  *(short*)(c+0x100) = (short)(((RandomIntInternal(RNG_STATE) >> 8) & 0x3f) + 0x64);
  *(int*)(c+0x35c) = 0x1000;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x300, (void*)data_ov062_0211ddf8[1], 0, 0x1000, 0);
  return 1;
}
}
