//cpp
extern "C" {
extern int func_ov002_020bea7c(char* c);
extern void func_ov002_020c43c4(void* c, int a);
extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
extern char data_ov002_0211067c[];
extern char data_ov002_0211013c[];
int func_ov002_020c9288(char* c){
  if (!func_ov002_020bea7c(c)) {
    if (*(unsigned char*)(c+0x6e6) == 0) {
      func_ov002_020c43c4(c, 6);
      *(unsigned char*)(c+0x6e6) = 1;
    } else if (*(unsigned char*)(c+0x706) != 0) {
      _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211067c);
    } else {
      _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211013c);
    }
  }
  return 0;
}
}
