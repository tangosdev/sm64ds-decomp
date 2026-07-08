//cpp
extern "C" {
struct State;
extern State _ZN6Player7ST_WAITE;
extern State _ZN6Player11ST_TELEPORTE;
extern int _ZN6Player7IsStateERNS_5StateE(void* c, State* st);
int _ZN6Player7CanWarpEv(void* c){
  if(_ZN6Player7IsStateERNS_5StateE(c, &_ZN6Player7ST_WAITE) || _ZN6Player7IsStateERNS_5StateE(c, &_ZN6Player11ST_TELEPORTE)) return 1;
  return 0;
}
}
