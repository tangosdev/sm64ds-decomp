//cpp
extern "C" {
extern int RandomIntInternal(int* seed);
extern int data_0209d4b8;

void func_ov006_020e0884(char* c, int i) {
  int idx = i * 0x24;
  unsigned int r;
  *(int*)(c + 0x4798 + idx) = 0;
  r = ((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
  *(int*)(c + 0x479c + idx) = (((r << 4) >> 15) << 4) + 0x300;
  *(unsigned char*)(c + 0x47ab + idx) = 1;
  r = ((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
  r = ((r << 5) >> 15) + 0x20;
  *(short*)(c + 0x47a4 + idx) = (unsigned char)r;
}
}
