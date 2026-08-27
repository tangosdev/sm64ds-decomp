//cpp
extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);
extern int RandomIntInternal(int* s);
extern int data_ov081_02128edc[];
extern int data_0209e650[];
}
extern "C" void func_ov081_021274c8(char* r4);
extern "C" void func_ov081_021274c8(char* r4){
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(r4+0xd4, (void*)data_ov081_02128edc[1], 0, 0x1000, 0);
  *(int*)(r4+0x130) = 0x1000;
  *(int*)(r4+0x98) = 0xa000;
  *(short*)(r4+0x3ee) = (short)RandomIntInternal(data_0209e650);
  *(unsigned char*)(r4+0x3f2) = (unsigned char)((unsigned int)RandomIntInternal(data_0209e650) % 0x78);
  *(int*)(r4+0x3e0) = 1;
}
