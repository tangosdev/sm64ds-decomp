void _ZN4cstd16reciprocal_asyncE5Fix12IiE(int a){
  *(volatile unsigned short*)0x4000280 = 1;
  *(volatile unsigned long long*)0x4000290 = ((unsigned long long)0x1000) << 32;
  *(volatile unsigned long long*)0x4000298 = (unsigned)a;
}
