//cpp
extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);
extern unsigned int RandomIntInternal(void* s);
extern int data_ov070_02123520[];
extern int RNG_STATE[];
int func_ov070_0211ffa8(char* c){
  *(short*)(c+0x300+0xe6) = (short)(((RandomIntInternal(RNG_STATE) >> 8) & 0xf) << 0xc);
  *(short*)(c+0x100) = (short)(((RandomIntInternal(RNG_STATE) >> 8) & 0x1f) + 0x32);
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x300, (void*)data_ov070_02123520[1], 0, 0x1000, 0);
  return 1;
}
}
