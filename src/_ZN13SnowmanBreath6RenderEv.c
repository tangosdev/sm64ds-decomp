extern unsigned char data_0209f2d8[];
extern void data_ov043_02112424(void*);
int _ZN13SnowmanBreath6RenderEv(char* c){
  int b = data_0209f2d8[0];
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
