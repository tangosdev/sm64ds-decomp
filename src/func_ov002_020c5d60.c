extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
extern int func_ov002_020c5dec(void*, int);
extern char data_ov002_02110124[];
extern char data_ov002_02110064[];
void func_ov002_020c5d60(void* c) {
  if (_ZN6Player7IsStateERNS_5StateE(c, data_ov002_02110124)) {
    if (*(unsigned char*)((char*)c+0x6e3) == 1) return;
  }
  if (*(int*)((char*)c+0x60) >= (int)0xf8ad0000) return;
  if (_ZN6Player7IsStateERNS_5StateE(c, data_ov002_02110064)) return;
  (*(unsigned short *)(((int)c + 0x6ce) & 0xFFFFFFFFFFFFFFFF)) |= 0x400;
  func_ov002_020c5dec(c, 1);
}
