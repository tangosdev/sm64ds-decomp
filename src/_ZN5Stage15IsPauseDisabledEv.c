extern int PLAYER_ARR[];
extern int func_ov002_020bd8ac(unsigned char *p);
int _ZN5Stage15IsPauseDisabledEv(void){
  unsigned char *p=*(unsigned char**)PLAYER_ARR;
  if(p!=0 && func_ov002_020bd8ac(p)==0) return 1;
  return 0;
}
