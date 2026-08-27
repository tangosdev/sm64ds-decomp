void func_ov007_020c29b4(char* c){
  unsigned short* s=(unsigned short*)c;
  *(int*)(c)=0;
  *(int*)(c+4)=0;
  s[4]=0;
  s[5]=0;
  *(int*)(c+0x18)=0;
  *(int*)(c+0x24)=0;
  *(int*)(c+0x28)=0;
  *(int*)(c+0x2c)=0;
  s[8]=0;
  s[7]=s[8];
  s[6]=s[7];
  s[0xb]=0;
  s[0xa]=s[0xb];
  s[9]=s[0xa];
}
