void func_ov006_02106758(char* c){
  int i, j;
  for(i = 0; i < 5; i++){
    for(j = 0; j < 0xa; j++){
      *(unsigned char*)(c + j + 0x4ec8) = 0xff;
    }
    c += 0xa;
  }
}
