void func_ov060_02115d50(unsigned char *p){
  if (*(unsigned short*)(p + 0x114) > 0x1e)
    *(unsigned*)(p + 0x110) = 0;
}
