int func_ov007_020be964(int *c){
  int *p = *(int**)((char*)c[1]+0x18);
  if(*p==0){ *p=1; return 1; }
  return *p;
}
