struct State { int a; int b; };
extern int _ZN6Player7IsStateERNS_5StateE(void *c, struct State *s);
extern struct State _ZN6Player14ST_CAMERA_ZOOME;
extern struct State _ZN6Player7ST_NULLE;
int _ZN6Player12Unk_020ca8f8Ev(void *c){
  if(_ZN6Player7IsStateERNS_5StateE(c, &_ZN6Player14ST_CAMERA_ZOOME)) return 1;
  if(_ZN6Player7IsStateERNS_5StateE(c, &_ZN6Player7ST_NULLE)) return 2;
  return 0;
}
