#ifdef _MSC_VER
void func_ov060_02111cc0(char* c, int n, int flags);
#else
void func_ov060_02111cc0(char* c, int n);
#endif
void func_ov060_02115314(char* c);
void func_ov060_021151d4(char* c);
void func_ov060_02115060(char* c);
void func_ov060_02114f88(char* c){
  c[0x422] = 0;
#ifdef _MSC_VER
  /* The ROM reaches the helper with r2 = 0 (the mov r2,#0 at 0x02114f90 that
     feeds the strb above is still live at the bl), and the helper takes its
     animation flags from r2 (src/func_ov060_02111cc0.cpp): pass that 0. */
  func_ov060_02111cc0(c, 0x10, 0);
#else
  func_ov060_02111cc0(c, 0x10);
#endif
  *(short*)(c+0x402) = 0;
  *(int*)(c+0x98) = 0;
  *(int*)(c+0xa8) = 0;
  if(*(unsigned char*)(c+0x414) == 0) func_ov060_02115314(c);
  else if(*(unsigned char*)(c+0x414) == 1) func_ov060_021151d4(c);
  else func_ov060_02115060(c);
}
