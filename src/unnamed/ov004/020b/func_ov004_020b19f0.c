typedef int s32;
s32 GetGameLanguage(void);
int RenderOamMainScreen(int a, int b, int c, int d, int e);
int func_ov004_020b1ea4(int a, int b, void* c, int d, int e, int f, int g);
extern char* data_ov004_020bbfa8[];
int func_ov004_020b19f0(void* self){
  int i = GetGameLanguage();
  RenderOamMainScreen(*(int*)(data_ov004_020bbfa8[i]+0x20), 0x20, 0xc, -1, -1);
  return func_ov004_020b1ea4(0x48, 0xc, self, 1, -1, 2, 0xa);
}
