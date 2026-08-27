extern int func_ov005_020c00b4(void *a, void *b);
int func_ov005_020c00e4(void *this){
  int i, j;
  for(i=0;i<4;i++){
    char *p = (char*)i + 0x18;
    for(j=6;j<9;j++){
      if(func_ov005_020c00b4(this, p)) return 1;
      p += 4;
    }
  }
  return 0;
}
