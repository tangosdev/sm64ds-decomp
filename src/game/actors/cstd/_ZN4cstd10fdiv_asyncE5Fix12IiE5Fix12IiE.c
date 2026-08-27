void _ZN4cstd10fdiv_asyncE5Fix12IiE5Fix12IiE(int a, int b){
  *(volatile unsigned short*)0x4000280 = 1;
  *(volatile unsigned long long*)0x4000290 = ((unsigned long long)(unsigned)a) << 32;
  *(volatile unsigned long long*)0x4000298 = (unsigned)b;
}
