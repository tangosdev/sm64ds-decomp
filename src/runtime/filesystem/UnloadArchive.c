typedef struct { int a; int b; int c,d,e; } AR;
extern void func_02018908(int,int);
extern AR data_0208ecf4[];
void UnloadArchive(int i){
  AR *p = &data_0208ecf4[i];
  int a = p->a;
  if(a == 0) return;
  func_02018908(a, p->b);
  p->a = 0;
  p->b = 0;
}
