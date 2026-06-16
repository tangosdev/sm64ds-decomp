//cpp
struct V { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(void*); };
extern "C" {
int func_ov065_02118da8(char* c){
  unsigned int b = *(unsigned char*)(c+0xef);
  if((b<<30)>>31){
    int buf[3];
    buf[0]=0x2c00; buf[1]=0x2c00; buf[2]=0x2c00;
    ((V*)(c+0xf0))->g5(buf);
  }
  return 1;
}
}
