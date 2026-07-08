struct State { int a; int b; };
extern int _ZN6Player7IsStateERNS_5StateE(void *c, struct State *s);
extern int _ZN6Player17SetNoControlStateEhih(void *c, unsigned char a, int b, unsigned char d);
extern struct State _ZN6Player13ST_NO_CONTROLE;
int func_ov002_020ca78c(void *c){
  if(_ZN6Player7IsStateERNS_5StateE(c, &_ZN6Player13ST_NO_CONTROLE)){
    if(*(unsigned char*)((char*)c+0x6e3)!=0x13 || *(unsigned char*)((char*)c+0x70b)==0) return 0;
  }
  return _ZN6Player17SetNoControlStateEhih(c,0x10,-1,1)!=0;
}
