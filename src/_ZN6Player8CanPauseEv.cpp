//cpp
extern "C" {
struct State;
extern State _ZN6Player7ST_SWIME;
extern State _ZN6Player21ST_METAL_WATER_GROUNDE;
extern int _ZN6Player7IsStateERNS_5StateE(void* c, State* st);
int _ZN6Player8CanPauseEv(void* c){
  if(*(unsigned char*)((char*)c+0x706)){
    if(_ZN6Player7IsStateERNS_5StateE(c, &_ZN6Player7ST_SWIME)) goto ret1a;
    if(_ZN6Player7IsStateERNS_5StateE(c, &_ZN6Player21ST_METAL_WATER_GROUNDE)) goto ret1a;
    goto ret0;
ret1a:
    return 1;
  }
  if(*(unsigned char*)((char*)c+0x6de)) goto ret0;
  if(*(unsigned char*)((char*)c+0x708)) goto ret0;
  if(*(unsigned char*)((char*)c+0x709)) goto ret0;
  return 1;
ret0:
  return 0;
}
}
