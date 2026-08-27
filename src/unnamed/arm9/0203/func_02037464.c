extern void func_020380c0(void *p);
void func_02037464(char *c){
  func_020380c0(c + 0x10);
  *(unsigned int*)(c+0x44) = 0x80000000;
  *(unsigned char*)(c+0x48) = 0;
}
