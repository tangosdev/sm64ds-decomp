void func_ov002_020bf340(int dummy, int*p, int delta, int limit){
  int neg = -limit;
  *p = *p + delta;
  if(*p < neg){ *p = neg; return; }
  if(*p > limit){ *p = limit; }
}
