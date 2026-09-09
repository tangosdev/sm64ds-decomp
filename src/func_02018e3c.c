/* recovered: the ROM reaches func_0205d644 with r0 and r1 still holding this
 * function's own two arguments (push {r4, lr} / bl func_0205d644 / movs r4, r0),
 * so both parameters are real and are forwarded. Every caller in the image
 * already passes two: func_020182bc, func_02018434, func_020189f0. */
extern int func_0205d644(void *out_file_id, const char *path);
extern void func_02018e68(void*);
extern void Crash(void);
extern int data_0208ecd8[];
int func_02018e3c(void *out_file_id, const char *path){
  int r = func_0205d644(out_file_id, path);
  if(!r){
    func_02018e68(data_0208ecd8);
    Crash();
  }
  return r;
}
