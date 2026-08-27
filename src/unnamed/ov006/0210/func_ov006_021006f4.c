#pragma opt_strength_reduction off
void func_ov006_021006f4(unsigned char* o){
  int i;
  for(i=0;i<3;i++){
    unsigned char* b = o + (i<<6);
    if(b[0x5000+0x294] != 0){
      b[0x5000+0x296] = 0xe;
      *(unsigned short*)(b + 0x5200 + 0x92) = 0x88;
    }
  }
}
