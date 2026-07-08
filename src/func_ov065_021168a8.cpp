//cpp
extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);
extern unsigned int RandomIntInternal(void* s);
extern int data_ov065_0211d600[];
extern int RNG_STATE[];
int func_ov065_021168a8(char* c){
  *(short*)(c+0x3e0) = (short)((RandomIntInternal(RNG_STATE) >> 8) << 0xc);
  *(short*)(c+0x100) = (short)(((RandomIntInternal(RNG_STATE) >> 8) & 0x1f) + 0x32);
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x300, (void*)data_ov065_0211d600[1], 0, 0x1000, 0);
  return 1;
}
}
