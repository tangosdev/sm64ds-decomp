struct State { int a; int b; };
extern int _ZN6Player7IsStateERNS_5StateE(void *c, struct State *s);
extern struct State _ZN6Player8ST_SHELLE;
int _ZN6Player9IsOnShellEv(void *c){
  if(_ZN6Player7IsStateERNS_5StateE(c, &_ZN6Player8ST_SHELLE)) return 1;
  *(int*)((char*)c+0x354)=0;
  return 0;
}
