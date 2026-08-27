void func_ov060_02111cc0(char* c, int n);
void func_ov060_02115314(char* c);
void func_ov060_021151d4(char* c);
void func_ov060_02115060(char* c);
void func_ov060_02114f88(char* c){
  c[0x422] = 0;
  func_ov060_02111cc0(c, 0x10);
  *(short*)(c+0x402) = 0;
  *(int*)(c+0x98) = 0;
  *(int*)(c+0xa8) = 0;
  if(*(unsigned char*)(c+0x414) == 0) func_ov060_02115314(c);
  else if(*(unsigned char*)(c+0x414) == 1) func_ov060_021151d4(c);
  else func_ov060_02115060(c);
}
