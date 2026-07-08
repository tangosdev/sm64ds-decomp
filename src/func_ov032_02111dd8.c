extern int RandomIntInternal(void*);
extern int RNG_STATE[];
int func_ov032_02111dd8(char* c){
  unsigned int r = RandomIntInternal(RNG_STATE);
  *(short*)(c+0x100) = ((r>>8)&0x1f)+0x14;
  *(int*)(c+0x3a8) = 0x1000;
  *(int*)(c+0xa4) = 0;
  *(int*)(c+0xa8) = 0;
  *(int*)(c+0xac) = 0;
  return 1;
}
