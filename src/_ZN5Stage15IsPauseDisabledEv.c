extern int data_0209f394[];
extern int func_ov002_020bd8ac(unsigned char *p);
int _ZN5Stage15IsPauseDisabledEv(void){
  unsigned char *p=*(unsigned char**)data_0209f394;
  if(p!=0 && func_ov002_020bd8ac(p)==0) return 1;
  return 0;
}
