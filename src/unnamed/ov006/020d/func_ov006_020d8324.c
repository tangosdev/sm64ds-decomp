void func_ov006_020d8324(int c, int i){
  if(*(unsigned short*)(c+(i<<6)+0x4690)!=0){
    unsigned short *h = (unsigned short*)((char*)(c+0x4690)+(i<<6));
    *h = *h - 1;
  } else {
    *(unsigned char*)(c+(i<<6)+0x4699)=1;
    *(unsigned char*)(c+(i<<6)+0x4694)=1;
    *(unsigned char*)(c+(i<<6)+0x4697)=1;
  }
}
