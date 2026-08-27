extern int _ZN8dActor_c10FindWithIDEj(unsigned int);
int func_ov002_020af1dc(char* c){
  int r=0;
  unsigned int id=*(unsigned int*)(c+0x134);
  if(id && (r=_ZN8dActor_c10FindWithIDEj(id)) && (*(int*)(c+0x130)&0x400000))
    return r;
  return 0;
}
