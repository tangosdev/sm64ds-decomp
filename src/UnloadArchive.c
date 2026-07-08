typedef struct { int a; int b; int c,d,e; } AR;
extern void func_02018908(int,int);
extern AR ARCHIVE_INFOS[];
void UnloadArchive(int i){
  AR *p = &ARCHIVE_INFOS[i];
  int a = p->a;
  if(a == 0) return;
  func_02018908(a, p->b);
  p->a = 0;
  p->b = 0;
}
