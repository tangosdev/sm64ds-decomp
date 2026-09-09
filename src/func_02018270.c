/* recovered: the handle rides through r0. The body saves r1 and r2 and calls
 * func_02018a24 without touching r0, so the handle its caller (LoadFileAt) left
 * there is the one translated to a file id. The no-argument call was byte-exact
 * and passed nothing. */
extern unsigned int func_02018a24(unsigned int handle);
extern void func_020185c0(void *buf, int x);
extern void func_020184e0(void *buf, int a, int b);
void func_02018270(unsigned int handle, int a, int b){
  char buf[0x44];
  unsigned int r = func_02018a24(handle);
  func_020185c0(buf, r);
  func_020184e0(buf, a, b);
}
