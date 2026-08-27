extern int RandomIntInternal(int* seed);
extern int data_0209d4b8;

void func_ov006_020e3db4(char* c, int i) {
  int idx = i * 0x24;
  unsigned int r;
  *(int*)(c + 0x48cc + idx) = 0;
  r = ((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
  *(int*)(c + 0x48d0 + idx) = (((r << 4) >> 15) << 4) + 0x300;
  *(unsigned char*)(c + 0x48df + idx) = 1;
  r = ((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
  r = ((r << 5) >> 15) + 0x20;
  *(short*)(c + 0x48d8 + idx) = (unsigned char)r;
}
