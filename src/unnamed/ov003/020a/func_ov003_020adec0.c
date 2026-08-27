extern int _ZN8SaveData19IsCharacterUnlockedEj(unsigned int);
extern unsigned char data_ov003_020b169c[];
int func_ov003_020adec0(char* c, unsigned int r6){
  unsigned char m = *(unsigned char*)(c+0x130);
  if(m == 4){
    unsigned char* p = data_ov003_020b169c;
    int r0;
    for(r0 = 0; r0 < 3; r0++){
      if(r6 == *p) return r0;
      p++;
    }
  } else if(m >= 2){
    int r5 = 0;
    int r4;
    for(r4 = 0; r4 < 3; r4++){
      if(_ZN8SaveData19IsCharacterUnlockedEj((unsigned int)r4) != 0){
        if((unsigned int)r4 == r6) return r5;
        r5++;
      }
    }
  }
  return 0;
}
