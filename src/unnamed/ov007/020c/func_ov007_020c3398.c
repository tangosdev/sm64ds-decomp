extern char* func_ov007_020c3df4(int,int);
char* func_ov007_020c3398(int a, int b){
  char* p=func_ov007_020c3df4(0,8);
  *(int*)p=a;
  *(short*)(p+4)=b;
  *(char*)(p+6)=0;
  return p;
}
