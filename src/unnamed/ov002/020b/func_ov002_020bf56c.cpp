//cpp
extern "C" {
extern int func_ov002_020c031c(void* c);
int func_ov002_020bf56c(void* a, int b){
  int m;
  switch (func_ov002_020c031c(a)) {
  case 4:
  case 5:
    m = 0x199;
    break;
  case 3:
    m = 0x599;
    break;
  default:
  case 0:
    m = 0x1000;
    break;
  case 1:
  case 2:
    m = 0x1000;
    break;
  }
  return (int)(((long long)b * m + 0x800) >> 12);
}
}
