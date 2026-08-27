extern int _ZN8SaveData19IsCharacterUnlockedEj(unsigned int);
extern unsigned char data_ov003_020b169c[];
int func_ov003_020adf50(char* c){
  unsigned char m = *(unsigned char*)(c+0x130);
  if(m == 4){
    return data_ov003_020b169c[*(unsigned char*)(c+0x134)];
  }
  if(m >= 2){
    int r5 = 0;
    int r4 = 0;
    for(; r4 < 3; r4++){
      if(_ZN8SaveData19IsCharacterUnlockedEj((unsigned int)r4) != 0){
        if(r5 == *(unsigned char*)(c+0x134)) return r4;
        r5++;
      }
    }
  }
  return 0;
}
