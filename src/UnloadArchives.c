typedef struct { int a; int b; int c,d,e; } AR;
extern void func_02018908(int,int);
extern AR ARCHIVE_INFOS[];
void UnloadArchives(void){
  unsigned int i;
  AR* p = ARCHIVE_INFOS;
  for (i = 0; i < 0xd; i++) {
    if (p->a != 0) {
      func_02018908(p->a, p->b);
      p->a = 0;
    }
    p++;
  }
}
