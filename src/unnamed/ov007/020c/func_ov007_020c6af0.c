extern void MultiCopy_Int(int *dst, int *src, int len);

struct M3 { int w[3]; };

struct O {
  char pad8[8];
  int n8;          /* 0x8 count */
  char pad[0x1c];
  int **p28;       /* 0x28 */
  int **p2c;       /* 0x2c */
  char pad2[8];
  unsigned short ***p38; /* 0x38 */
  char pad3[0x34];
  int v70[3];      /* 0x70 */
  int v7c[3];      /* 0x7c */
};

void func_ov007_020c6af0(struct O *c){
  int i;
  for(i = 0; i < c->n8; i++){
    MultiCopy_Int(c->p28[i], c->p2c[i], ((unsigned short*)c->p38[i])[4] * 0xc);
  }
  *(struct M3*)c->v7c = *(struct M3*)c->v70;
}
