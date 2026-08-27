extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);
void func_ov006_0211e318(char *c){
  if (*(unsigned char*)(c + 0x4c1c) == 0) return;
  if (*(unsigned short*)(c + 0x4c14) == 0) return;
  *(unsigned char*)(((int)c + 0x4c1a)) = *(unsigned char*)(((int)c + 0x4c1a)) + 1;
  if (*(unsigned char*)(c + 0x4c1a) >= 0x3c){
    *(unsigned char*)(c + 0x4c1a) = 0;
    *(unsigned short*)(((int)c + 0x4c14)) = *(unsigned short*)(((int)c + 0x4c14)) - 1;
    if (*(short*)(c + 0x4c14) < 0) *(short*)(c + 0x4c14) = 0;
    if (*(unsigned short*)(c + 0x4c14) != 0)
      _ZN5Sound12PlayBank2_2DEj(0xa7);
    else
      _ZN5Sound12PlayBank2_2DEj(0xa6);
  }
  if (*(unsigned short*)(c + 0x4c14) == 0){
    *(unsigned short*)(c + 0x4c14) = 0;
    *(unsigned char*)(c + 0x4c1a) = 0;
  }
}
