//cpp
extern "C" {
extern void* _ZN6Memory8AllocateEj(unsigned int);
extern void func_0205a588(void*, int, unsigned int);
extern void *data_0209d3b8;
extern int data_0209d3b0;
int func_02018a58(int n){
  unsigned int sz = n*2;
  void *p = _ZN6Memory8AllocateEj(sz);
  data_0209d3b8 = p;
  func_0205a588(p, 0, sz);
  data_0209d3b0 = n;
  return 1;
}
}
