extern unsigned char CURRENT_GAMEMODE[];
extern void data_ov043_02112424(void*);
int func_ov027_02112740(char* c){
  int b = CURRENT_GAMEMODE[0];
  b = (b == 2);
  if(b) return 1;
  {
    int i;
    char* p = c + 0xd4;
    for(i=0;i<0x32;i++){
      data_ov043_02112424(p);
      p += 0x60;
    }
  }
  return 1;
}
