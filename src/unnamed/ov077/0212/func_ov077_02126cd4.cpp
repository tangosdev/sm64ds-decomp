//cpp
extern "C" {
extern unsigned int RandomIntInternal(void* s);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);
extern int data_0209e650[];
extern int data_ov077_02127c90[];
int func_ov077_02126cd4(char* c){
  *(short*)(c+0x400+0x20) = (short)(((RandomIntInternal(data_0209e650) >> 8) & 0xf) << 0xc);
  *(short*)(c+0x100) = (short)(((RandomIntInternal(data_0209e650) >> 8) & 0x3f) + 0xaa);
  *(int*)(c+0x39c) = 0x1000;
  *(int*)(c+0x98) = 0x8000;
  *(int*)(c+0x41c) = 0;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x340, (void*)data_ov077_02127c90[1], 0, 0x1000, 0);
  return 1;
}
}
