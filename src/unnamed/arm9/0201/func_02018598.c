struct P { int a, b; };
extern int data_0209d3b4;
void func_02018598(struct P *c, int arg){
  struct P t;
  t.b = arg;
  t.a = data_0209d3b4;
  *c = t;
}
