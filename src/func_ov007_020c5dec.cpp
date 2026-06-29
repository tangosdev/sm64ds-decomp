//cpp
extern "C" void func_ov007_020bfe4c(void *o, int x, int y, int z, void *out);

struct Vec3 { int w[3]; };

struct Obj {
  char pad[8];
  unsigned short count;   /* 0x8 */
  char pad2[0x24 - 0xa];
  int *f24;               /* 0x24 */
  int *f28;               /* 0x28 */
};

struct ObjA8 {
  char pad[0x2c];
  int f2c;                /* 0x2c */
};

struct C {
  char pad0[8];
  int n;                  /* 0x8 */
  char pad1[0x28 - 0xc];
  Vec3 **a28;             /* 0x28 */
  Vec3 **a2c;             /* 0x2c */
  char pad2[0x38 - 0x30];
  Obj **a38;              /* 0x38 */
  char pad3[0x70 - 0x3c];
  Vec3 f70;               /* 0x70 */
  Vec3 f7c;               /* 0x7c */
  char pad4[0xa8 - 0x88];
  ObjA8 *a8;              /* 0xa8 */
};

extern "C" void func_ov007_020c5dec(C *c){
  int i;
  for (i = 0; i < c->n; i++) {
    int j;
    for (j = 0; j < c->a38[i]->count; j++) {
      ObjA8 *o = c->a8;
      Obj *obj = c->a38[i];
      func_ov007_020bfe4c(o, obj->f24[j], obj->f28[j], -o->f2c, &c->a2c[i][j]);
      {
        Vec3 *dst = &c->a28[i][j];
        Vec3 *src = &c->a2c[i][j];
        *dst = *src;
      }
    }
  }
  c->f70 = c->f7c;
  {
    int *p5c = (int*)((char*)c + 0x5c);
    int *p58 = (int*)((char*)c + 0x58);
    int *p68 = (int*)((char*)c + 0x68);
    int *p64 = (int*)((char*)c + 0x64);
    int *p60 = (int*)((char*)c + 0x60);
    *p5c = -1;
    *p58 = *p5c;
    *p68 = 0;
    *p64 = *p68;
    *p60 = *p64;
  }
}
