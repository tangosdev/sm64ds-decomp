//cpp
extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);
extern unsigned int RandomIntInternal(void* s);
extern int data_ov077_02127c90[];
extern int RNG_STATE[];
int func_ov077_02126930(char* c){
  *(short*)(c+0x100) = (short)(((RandomIntInternal(RNG_STATE) >> 8) & 0x3f) + 0xaa);
  *(int*)(c+0x98) = 0x6000;
  *(short*)(c+0x400+0x22) = 0;
  *(int*)(c+0x41c) = 0;
  *(int*)(c+0x39c) = 0x2000;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x340, (void*)data_ov077_02127c90[1], 0, 0x1000, 0);
  return 1;
}
}
