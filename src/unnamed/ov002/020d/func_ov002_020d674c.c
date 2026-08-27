struct O;
typedef int (*VFN)(struct O*);
struct O { VFN* vt; };
int func_ov002_020d674c(char* c){
  struct O* o = *(struct O**)(c+0x360);
  if (o == 0) return 0;
  return o->vt[0x12](o) == 6;
}
