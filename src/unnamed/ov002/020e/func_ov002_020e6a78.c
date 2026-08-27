extern unsigned short Color_Interp(void* a, unsigned short x, unsigned short y, int z);
extern void func_020555a4(void* p);
extern unsigned short data_ov002_0210a760[];
void func_ov002_020e6a78(char* c, int b){
  unsigned short* src=(unsigned short*)c;
  unsigned short* tbl=data_ov002_0210a760;
  unsigned short* dst=(unsigned short*)(c+0x40);
  int i;
  for(i=0;i<0x20;i++){
    *dst++ = Color_Interp(c, *src++, *tbl++, b);
  }
  func_020555a4(c+0x40);
}
