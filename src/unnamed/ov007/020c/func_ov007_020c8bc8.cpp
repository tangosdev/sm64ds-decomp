//cpp
typedef long long s64;
extern "C" void func_ov007_020c8bc8(char* r0, int t){
  unsigned char c1 = *(unsigned char*)(r0+0x32);
  unsigned char c0 = *(unsigned char*)(r0+0x30);
  int v2 = c1;
  if (t < 0x1000) {
    if (t <= 0) v2 = c0;
    else v2 = (int)(((s64)(0x1000 - t) * (c0 << 12) + (s64)t * (c1 << 12)) >> 12) >> 12;
  }
  {
    unsigned char d1 = *(unsigned char*)(r0+0x33);
    unsigned char d0 = *(unsigned char*)(r0+0x31);
    int vip = d1;
    if (t < 0x1000) {
      if (t <= 0) vip = d0;
      else vip = (int)(((s64)(0x1000 - t) * (d0 << 12) + (s64)t * (d1 << 12)) >> 12) >> 12;
    }
    if (*(int*)r0 == 3) vip = 0x10 - v2;
    if (*(int*)(r0+4) & 1) *(volatile unsigned short*)0x4000052 = (unsigned short)(v2 | (vip << 8));
    if (*(int*)(r0+4) & 2) *(volatile unsigned short*)0x4001052 = (unsigned short)(v2 | (vip << 8));
  }
}
