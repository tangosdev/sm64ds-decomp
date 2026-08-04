void func_ov006_020d8904(char* p){
  int i;
  for (i = 0; i < 0x70; i++) {
    if (((unsigned char (*)[0x40])(p + 0x4000))[i][0x698] != 0) {
      if (((unsigned char (*)[0x40])(p + 0x4000))[i][0x697] == 6)
        ((unsigned char (*)[0x40])(p + 0x4000))[i][0x699] = 0;
    }
  }
}
