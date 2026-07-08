extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
extern int func_ov002_020c5dec(void*, int);
extern char _ZN6Player11ST_DEAD_PITE[];
extern char _ZN6Player17ST_IN_YOSHI_MOUTHE[];
void func_ov002_020c5d60(void* c) {
  if (_ZN6Player7IsStateERNS_5StateE(c, _ZN6Player11ST_DEAD_PITE)) {
    if (*(unsigned char*)((char*)c+0x6e3) == 1) return;
  }
  if (*(int*)((char*)c+0x60) >= (int)0xf8ad0000) return;
  if (_ZN6Player7IsStateERNS_5StateE(c, _ZN6Player17ST_IN_YOSHI_MOUTHE)) return;
  (*(unsigned short *)(((int)c + 0x6ce) & 0xFFFFFFFFFFFFFFFF)) |= 0x400;
  func_ov002_020c5dec(c, 1);
}
