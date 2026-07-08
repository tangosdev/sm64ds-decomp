//cpp
extern "C" {
extern int _ZN6Player14ST_LEVEL_ENTERE[];
extern int _ZN6Player12ST_DEBUG_FLYE[];
extern int _ZN6Player7IsStateERNS_5StateE(char*c, void*s);
int _ZN6Player20IsStateEnteringLevelEv(char*c){
  if(_ZN6Player7IsStateERNS_5StateE(c, _ZN6Player14ST_LEVEL_ENTERE)) return 1;
  return _ZN6Player7IsStateERNS_5StateE(c, _ZN6Player12ST_DEBUG_FLYE);
}
}
