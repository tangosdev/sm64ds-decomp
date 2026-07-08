//cpp
extern "C" {
extern int _ZN6Player7IsStateERNS_5StateE(void *c, void *s);
extern int _ZN6Player11ChangeStateERNS_5StateE(void *c, void *s);
extern char _ZN6Player17ST_IN_YOSHI_MOUTHE[];
extern char _ZN6Player7ST_WALKE[];
int func_ov002_020d5ed0(char* c){
  if (!_ZN6Player7IsStateERNS_5StateE(c, _ZN6Player17ST_IN_YOSHI_MOUTHE)) {
    *(int*)(c+0xd0) = 0;
    _ZN6Player11ChangeStateERNS_5StateE(c, _ZN6Player7ST_WALKE);
    return 0;
  }
  *(unsigned char*)(c+0x6e3) = 1;
  *(unsigned short*)(c+0x600+0xa4) = 0x5a;
  *(unsigned char*)(c+0x6f5) = 0;
  return 1;
}
}
