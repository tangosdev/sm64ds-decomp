extern void func_ov002_020bda48(char* c);
extern void _ZN6Player17SetNoControlStateEhih(void* thiz, unsigned char a, int b, unsigned char d);
extern void func_02012790(int a);
extern void func_ov002_020bd9ec(char* c, unsigned int r4);
extern char* data_0209f318;
void func_ov002_020bdf8c(char* c){
  func_ov002_020bda48(c);
  *(unsigned char*)(c+0x6f2) = 0;
  if(*(unsigned char*)(c+0x703) == 0){
    _ZN6Player17SetNoControlStateEhih(c, 0xd, -1, 0);
    func_02012790(0x2e);
  }
  *(unsigned char*)(c+0x703) = 1;
  *(unsigned short*)(c+0x600+0xc2) = 0x258;
  (*(unsigned int *)(((int)data_0209f318 + 0x154) & 0xFFFFFFFFFFFFFFFF)) |= 0x80;
  func_ov002_020bd9ec(c, 0x31);
}
