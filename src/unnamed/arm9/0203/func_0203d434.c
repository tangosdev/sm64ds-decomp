typedef int Fix12i;
extern void func_0203d480(Fix12i* out, Fix12i* in);
int func_0203d434(Fix12i* in){
  Fix12i out[3];
  if (in[0] == 0 && in[1] == 0) return 0;
  func_0203d480(out, in);
  return 1;
}
