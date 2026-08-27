//cpp
extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);
extern unsigned int RandomIntInternal(void* s);
extern int data_ov094_02136af0[];
extern int data_0209e650[];
int func_ov094_02135e64(char* c){
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x30c, (void*)data_ov094_02136af0[1], 0, 0x1000, 0);
  *(short*)(c+0x3ec) = (short)((RandomIntInternal(data_0209e650) >> 8) << 0xc);
  *(short*)(c+0x100) = (short)(((RandomIntInternal(data_0209e650) >> 8) & 0x3f) + 0x32);
  *(int*)(c+0x3f0) = 0x1000;
  return 1;
}
}
