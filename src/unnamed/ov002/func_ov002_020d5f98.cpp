//cpp
extern "C" {
extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
extern int RandomIntInternal(int* seed);
extern char data_ov002_02110064[];
extern int data_ov002_0210e160[];
int func_ov002_020d5f98(char* c, unsigned char* a, unsigned char* b){
  if (_ZN6Player7IsStateERNS_5StateE(c, data_ov002_02110064)
      && *(unsigned char*)(c+0x6e3) == 3
      && *(unsigned char*)(c+0x6e5) != 0) {
    *a = ((unsigned)RandomIntInternal(data_ov002_0210e160) >> 4) & 3;
    *b = *(unsigned char*)(c+0x6e5);
    return 1;
  }
  return 0;
}
}
