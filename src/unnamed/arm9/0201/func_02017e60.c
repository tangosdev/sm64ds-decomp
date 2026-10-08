struct S { int a, b, c; };
extern struct S data_0209d3c4[];
void func_02017e60(void){
  unsigned int i;
  struct S *p = data_0209d3c4;
  for(i=0;i<12;i++,p++){
    p->a=-1; p->b=0; p->c=0;
  }
}
