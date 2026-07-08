extern int _ZN6Player7IsStateERNS_5StateE(void *c, void *s);
extern int _ZN6Player11ChangeStateERNS_5StateE(void *c, void *s);
extern char _ZN6Player15ST_TORNADO_SPINE[];
extern char _ZN6Player15ST_CRAZED_CRATEE[];
int func_ov002_020de33c(char* c, int a){
  if(*(unsigned char*)(c+0x709)==0
     && !_ZN6Player7IsStateERNS_5StateE(c, _ZN6Player15ST_TORNADO_SPINE)
     && !_ZN6Player7IsStateERNS_5StateE(c, _ZN6Player15ST_CRAZED_CRATEE)){
    goto ok;
  }
  return 0;
ok:
  *(int*)(c+0x364) = a;
  _ZN6Player11ChangeStateERNS_5StateE(c, _ZN6Player15ST_TORNADO_SPINE);
  return 1;
}
