extern void *func_ov007_020c2b9c(int count, int x);
extern void func_ov007_020c2d44(void *obj, int i);

struct O {
  char pad[0x24];
  int *p24;   /* 0x24 */
  int *p28;   /* 0x28 */
};

void *func_ov007_020c2acc(int count, unsigned char *a, unsigned char *b, int x){
  struct O *o = (struct O*)func_ov007_020c2b9c(count, x);
  int i;
  for(i = 0; i < count; i++){
    o->p24[i] = a[i] << 0xc;
    o->p28[i] = b[i] << 0xc;
    func_ov007_020c2d44(o, i);
  }
  return o;
}
