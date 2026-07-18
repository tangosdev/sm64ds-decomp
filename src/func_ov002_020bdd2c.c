extern void func_ov002_020db8bc(unsigned char* p, unsigned char val);
extern void func_ov002_020bd984(void* c, int x);
extern void* data_0209f318;

void func_ov002_020bdd2c(void* c) {
  if (*(unsigned char*)((char*)c+0x703) == 1) {
    func_ov002_020db8bc((unsigned char*)c, 3);
    {
      int* base = (int*)(((int)data_0209f318 + 0x154) & 0xFFFFFFFFFFFFFFFF);
      *base &= ~0x80;
    }
    func_ov002_020bd984(c, 0x31);
    *(int*)((char*)c+0x638) = 0;
    *(int*)((char*)c+0x634) = *(int*)((char*)c+0x638);
    *(int*)((char*)c+0x628) = 0;
    *(short*)((char*)c+0x6d0) = 0;
  }
  *(unsigned char*)((char*)c+0x703) = 0;
}
