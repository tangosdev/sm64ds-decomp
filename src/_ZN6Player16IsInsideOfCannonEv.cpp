//cpp
extern "C" {
struct State;
extern State _ZN6Player9ST_CANNONE;
extern int _ZN6Player7IsStateERNS_5StateE(void* c, State* st);
int _ZN6Player16IsInsideOfCannonEv(char* c){
  if(!_ZN6Player7IsStateERNS_5StateE(c, &_ZN6Player9ST_CANNONE)) return 0;
  return *(unsigned char*)(c+0x6e3)!=2 ? 1 : 0;
}
}
