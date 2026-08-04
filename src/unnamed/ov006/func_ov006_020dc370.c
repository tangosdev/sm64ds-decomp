void func_ov006_020dc370(char *c){
  if(*(unsigned char*)(c+0x51de)!=0) return;
  if(*(unsigned char*)(c+0x51a1)==2){
    *(short*)(c+0x519c)=0x3c;
    return;
  }
  *(short*)(c+0x519c)=0x3c;
  *(char*)(c+0x51a1)=2;
  *(char*)(c+0x51a4)=0;
  *(short*)(c+0x519e)=0;
}
