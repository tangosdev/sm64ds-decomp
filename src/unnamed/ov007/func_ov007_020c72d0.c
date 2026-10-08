extern void func_ov007_020c7804(void *p);

struct T {
  int n0;     /* 0x0 */
  int n4;     /* 0x4 */
  void **a8;  /* 0x8 */
  void **ac;  /* 0xc */
};

void func_ov007_020c72d0(struct T *c){
  int i;
  for(i = 0; i < c->n4; i++){
    func_ov007_020c7804(c->a8[i]);
  }
  for(i = 0; i < c->n0; i++){
    func_ov007_020c7804(c->ac[i]);
  }
}
