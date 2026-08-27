extern int func_0200e6d8(int);
extern int func_0200e5fc(int);
extern int func_ov002_020bd664(int a,void* b,void* c,void* d);
int func_0200e5ac(void* c,void* r1,void* r2){
  int r=func_0200e6d8(*(unsigned char*)((char*)c+1));
  if(r==0){
    r=func_0200e5fc(*(unsigned char*)((char*)c+1));
    if(r==0) return 0;
  }
  return func_ov002_020bd664(r,c,r1,r2);
}
