extern void _ZN5dBgPi5ResetEv(void *p);
void func_02037464(char *c){
  _ZN5dBgPi5ResetEv(c + 0x10);
  *(unsigned int*)(c+0x44) = 0x80000000;
  *(unsigned char*)(c+0x48) = 0;
}
