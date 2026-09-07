void func_ov006_020d8324(char *c, int i){
  char *b = c + (i<<6);
  if(*(unsigned short*)(b+0x4690)!=0){
    unsigned short *h = (unsigned short*)((c+0x4690)+(i<<6));
    *h = *h - 1;
  } else {
    *(unsigned char*)(b+0x4699)=1;
    *(unsigned char*)(b+0x4694)=1;
    *(unsigned char*)(b+0x4697)=1;
  }
}
