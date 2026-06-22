int func_ov006_02111d74(char* c){
  if(*(unsigned char*)(c+0x30) == 0) return 1;
  if(*(unsigned char*)(c+0x3a) == 1) return 1;
  int v = *(int*)(c+0xc);
  if(v < -0xa0000) return 1;
  if(v < 0){
    if(*(int*)(c+8) < 0xd0000) return 1;
  }
  return 0;
}
