//cpp
extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);
extern unsigned int RandomIntInternal(void* s);
extern int data_ov065_0211d690[];
extern int RNG_STATE[];
int func_ov065_021176fc(char* c){
  *(int*)(c+0x9c) = 0;
  *(char*)(c+0x43c) = 1;
  *(int*)(c+0xb0) = 0x10000003;
  *(short*)(c+0x100) = 0x64;
  *(int*)(c+0x35c) = 0x2000;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x300, (void*)data_ov065_0211d690[1], 0, 0x1000, 0);
  *(short*)(c+0x400+0x34) = (short)(((RandomIntInternal(RNG_STATE) >> 8) & 0xf) << 0xc);
  return 1;
}
}
