extern void func_ov062_02118a00(void *c);
extern void func_ov062_02118de8(void *c);
extern void func_ov062_02118cdc(void *c);
extern void func_ov062_02118b4c(void *c);
extern void func_ov062_02118a50(void *c);
void func_ov062_02117acc(char *c){
  switch(*(int*)(c+0x38c)){
  case 0: func_ov062_02118a00(c); break;
  case 1: func_ov062_02118de8(c); break;
  case 2: func_ov062_02118cdc(c); break;
  case 3: func_ov062_02118b4c(c); break;
  case 4: func_ov062_02118a50(c); break;
  }
}
