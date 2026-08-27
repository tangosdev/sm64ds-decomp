//cpp
extern "C" {
extern int _ZN6Player7IsStateERNS_5StateE(void *c, void *s);
extern int _ZN6Player11ChangeStateERNS_5StateE(void *c, void *s);
extern char data_ov002_02110064[];
extern char data_ov002_0211013c[];
int func_ov002_020d5ed0(char* c){
  if (!_ZN6Player7IsStateERNS_5StateE(c, data_ov002_02110064)) {
    *(int*)(c+0xd0) = 0;
    _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211013c);
    return 0;
  }
  *(unsigned char*)(c+0x6e3) = 1;
  *(unsigned short*)(c+0x600+0xa4) = 0x5a;
  *(unsigned char*)(c+0x6f5) = 0;
  return 1;
}
}
