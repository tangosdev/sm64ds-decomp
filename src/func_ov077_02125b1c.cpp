//cpp
extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* ma, void* bca, int a, int f, unsigned int j);
extern void func_02035684(int* p, int v);
extern int RandomIntInternal(int* seed);
extern void* data_ov077_02127c14[];
extern int data_0209e650;
void func_ov077_02125b1c(char* c) {
  *(int*)(c + 0x9c) = -0x2000;
  *(int*)(c + 0xa0) = -0x3c000;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x124, data_ov077_02127c14[1], 0, 0x1000, 0);
  *(int*)(c + 0x180) = 0x1000;
  func_02035684((int*)(c + 0x1e4), 0x28000);
  *(int*)(c + 0x98) = 0x1b33;
  *(short*)(c + 0x3e6) = RandomIntInternal(&data_0209e650);
  *(unsigned char*)(c + 0x3e8) = RandomIntInternal(&data_0209e650);
  *(int*)(c + 0x3d8) = 1;
}
}
