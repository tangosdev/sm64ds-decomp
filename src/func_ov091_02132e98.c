extern int DecIfAbove0_Byte(void*);
extern int RandomIntInternal(void*);
extern int RNG_STATE[];
void func_ov091_02132e98(char *c){
  if(DecIfAbove0_Byte(c+0x39e)) return;
  *(int*)(c+0x398)=4;
  unsigned int r=RandomIntInternal(RNG_STATE);
  unsigned int v=r>>16;
  c[0x39e]=(char)(v%10+0x14);
}
