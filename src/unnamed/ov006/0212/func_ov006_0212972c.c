void func_ov006_0212972c(char* c, int idx){
  int off = idx * 0x24;
  short* h = (short*)(c + 0xbeac + off);
  if (*(unsigned short*)h != 0) {
    *h = (short)(*(unsigned short*)h - 1);
    if (*h < 0) *h = 0;
    return;
  }
  {
    int* w = (int*)(c + 0xbea0 + off);
    if (*w > 0x100) {
      *w = *w - 0x10;
      if ((short)*w < 0x100) *w = 0x100;
      return;
    }
  }
  *(unsigned char*)(c + 0xb000 + off + 0xeb1) = 0;
}
