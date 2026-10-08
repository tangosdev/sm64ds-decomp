extern unsigned short data_ov002_0210a760[];
void func_ov002_020e69c0(void){
  unsigned short* dst = data_ov002_0210a760;
  unsigned short a = *dst;
  unsigned short b = dst[1];
  unsigned short* src = (unsigned short*)((char*)dst + 4);
  int i;
  for(i=2; i<0x20; i++){ *dst++ = *src++; }
  dst[0]=a; dst[1]=b;
}
