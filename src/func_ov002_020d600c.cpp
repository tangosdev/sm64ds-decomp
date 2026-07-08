//cpp
extern "C" {
struct State;
extern State _ZN6Player17ST_IN_YOSHI_MOUTHE;
extern int _ZN6Player7IsStateERNS_5StateE(void* c, State* st);
int func_ov002_020d600c(char* c){
  if(_ZN6Player7IsStateERNS_5StateE(c, &_ZN6Player17ST_IN_YOSHI_MOUTHE)){
    if(*(unsigned char*)(c+0x6e3)==1) return 1;
  }
  return 0;
}
}
