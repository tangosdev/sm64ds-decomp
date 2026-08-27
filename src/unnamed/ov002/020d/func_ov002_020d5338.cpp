//cpp
extern "C" {
extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
extern char data_ov002_02110304[];
extern char data_ov002_02110484[];
extern char data_ov002_021100f4[];
int func_ov002_020d5338(char* c){
  if (_ZN6Player7IsStateERNS_5StateE(c, data_ov002_02110304)
      || _ZN6Player7IsStateERNS_5StateE(c, data_ov002_02110484)
      || *(unsigned char*)(c+0x6f9) != 0
      || *(unsigned char*)(c+0x709) != 0) return 0;
  _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_021100f4);
  return 1;
}
}
