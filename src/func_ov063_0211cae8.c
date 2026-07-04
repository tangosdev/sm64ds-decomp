void func_02012790(int x);
void func_ov063_0211cae8(unsigned char* c, int m){
  unsigned char* p = (unsigned char*)(((long long)(int)(c + 0x157)) & 0xFFFFFFFFFFFFFFFFLL);
  *p |= m;
  unsigned char v = c[0x157] & 7;
  if(v!=1 && v!=3 && v!=7){ func_02012790(0xe); return; }
  func_02012790(0x26);
}
