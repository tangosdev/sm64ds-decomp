int func_ov002_020f035c(unsigned int sel, int r1){
  int v;
  switch(sel){
  case 4: v=0xff0; break;
  case 5: v=0xfff; break;
  case 3: v=0xfc1; break;
  default: v=0xf74; break;
  case 1:
  case 2: v=0xf08; break;
  }
  return r1 <= v;
}
