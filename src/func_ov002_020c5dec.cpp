//cpp
extern "C" {
extern int _ZN6Player11ST_DEAD_PITE[];
extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
int func_ov002_020c5dec(char* c, int r1){
  if (r1 != 1) {
    if (*(unsigned char*)(c+0x709) != 0) return 0;
  }
  *(unsigned char*)(c+0x6e3) = (unsigned char)r1;
  _ZN6Player11ChangeStateERNS_5StateE(c, _ZN6Player11ST_DEAD_PITE);
  return 1;
}
}
