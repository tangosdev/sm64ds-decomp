/* recovered: both arguments ride through. The body is push {r4, lr} / bl
 * func_0205d518 / movs r4, r0, so r0 (the FSFile) and r1 (the path) reach the
 * callee untouched. func_02018934 already calls this with two. */
extern int func_0205d518(void *file, const char *path);
extern void func_02018e68(void*);
extern void Crash(void);
extern int data_0208eb8c[];
int func_02018d98(void *file, const char *path){
  int r = func_0205d518(file, path);
  if(!r){
    func_02018e68(data_0208eb8c);
    Crash();
  }
  return r;
}
