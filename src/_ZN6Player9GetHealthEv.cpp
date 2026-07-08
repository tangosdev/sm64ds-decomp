//cpp
extern short HEALTH_ARR[];
extern "C" int _ZN6Player9GetHealthEv(char* c){
  unsigned char i = *(unsigned char*)(c+0x6d8);
  return (signed char)(HEALTH_ARR[i] >> 8);
}
