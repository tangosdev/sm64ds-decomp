extern int func_0200e55c(int);
extern int func_0200e4e4(int);
extern int func_ov002_020f7d74(int a,void* b,void* c,void* d);
int func_0200e494(void* c,void* r1,void* r2){
  int r=func_0200e55c(*(unsigned char*)((char*)c+1));
  if(r==0){
    r=func_0200e4e4(*(unsigned char*)((char*)c+1));
    if(r==0) return 0;
  }
  return func_ov002_020f7d74(r,c,r1,r2);
}
