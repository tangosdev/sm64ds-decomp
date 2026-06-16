typedef unsigned short u16;
typedef unsigned char u8;
void func_ov001_020ab5b0(char* r0, int r1, short r2, short r3, short s4, short s5){
  *(int*)(r0+0x14) = r1;
  *(u16*)(r0+4) = r2;
  *(u16*)(r0+6) = r3;
  *(u16*)(r0+8) = s4;
  *(u16*)(r0+0xa) = s5;
  *(int*)(r0+0xc) = 0;
  *(u8*)(r0+0x10) = 0;
  *(u8*)(r0+0x12) = 0;
  if(*(int*)(r0+0x14) >= 1)
    *(u8*)(r0+0x11) = 1;
  *(int*)(r0+0x18) = 0xa;
}
