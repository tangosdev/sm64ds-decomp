//cpp
struct C;
typedef void (C::*PMF)(int);
extern PMF data_ov006_02142cc0[];
extern "C" void func_ov006_0211a2c4(C* c, int i){
  unsigned char sel = *((unsigned char*)((char*)c + i*0x24 + 0x51d1));
  (c->*data_ov006_02142cc0[sel])(i);
}
