void func_ov006_02104fb4(unsigned char* c){
  unsigned char* slot = c + 0x4600;
  if(*(unsigned short*)(slot + 0x70) != 0){
    unsigned short* p = (unsigned short*)(c + 0x4670);
    *p = *p - 1;
    if(*(short*)(slot + 0x70) < 0) *(unsigned short*)(slot + 0x70) = 0;
  } else {
    *(int*)(c + 0x4668) = -0x3000;
    *(unsigned char*)(c + 0x4675) = 3;
  }
}
