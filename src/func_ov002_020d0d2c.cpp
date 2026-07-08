//cpp
extern "C" {
struct State;
extern State _ZN6Player13ST_LEDGE_HANGE;
extern State _ZN6Player13ST_LEDGE_GRABE;
extern int _ZN6Player7IsStateERNS_5StateE(void* c, State* st);
int func_ov002_020d0d2c(void* c){
  if(_ZN6Player7IsStateERNS_5StateE(c, &_ZN6Player13ST_LEDGE_HANGE) || _ZN6Player7IsStateERNS_5StateE(c, &_ZN6Player13ST_LEDGE_GRABE)) return 1;
  return 0;
}
}
