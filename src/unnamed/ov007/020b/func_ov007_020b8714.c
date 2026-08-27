typedef struct Obj {
  int x0;                 /* 0x00 */
  char pad4[12];          /* 0x04..0x0f */
  int x10;                /* 0x10 */
  char pad14[58];         /* 0x14..0x4d */
  unsigned char x4e;      /* 0x4e */
  char pad4f[1];          /* 0x4f */
  int x50;                /* 0x50 */
  int x54;                /* 0x54 */
  char pad58[4];          /* 0x58..0x5b */
  int* x5c;               /* 0x5c */
  char pad60[12];         /* 0x60..0x6b */
  int x6c;                /* 0x6c */
  char pad70[20];         /* 0x70..0x83 */
  void* x84;              /* 0x84 */
} Obj;
struct Ddc { int a, b, c, d, e; };
extern Obj* data_ov007_0210346c;
extern Obj* data_ov007_02103460;
extern int data_ov007_02103484;
extern int data_ov007_02103470;
extern int data_ov007_02103474;
extern struct Ddc data_ov007_02102ddc;
extern Obj* func_ov007_020c44e4(int n);

void func_ov007_020b8714(int a0, int a1, int a2) {
  if (data_ov007_0210346c == 0 && data_ov007_02103460 == 0) {
    data_ov007_0210346c = func_ov007_020c44e4(1);
    data_ov007_0210346c->x10 = 0xffffec04;
    data_ov007_0210346c->x50 = 0x18000;
    data_ov007_0210346c->x0 |= 1;
    data_ov007_0210346c->x4e = 0x1d;
    data_ov007_0210346c->x54 = 0x800;
    data_ov007_02103460 = func_ov007_020c44e4(1);
    data_ov007_02103460->x10 = 0xffffec04;
    data_ov007_02103460->x4e = 0x1e;
    data_ov007_02103460->x6c = 1;
    data_ov007_02103460->x0 |= 8;
    data_ov007_02103460->x84 = &data_ov007_02103484;
    *data_ov007_02103460->x5c = a1;
  }
  data_ov007_02103470 = a0;
  data_ov007_02102ddc.a = 0;
  data_ov007_02102ddc.b = 4;
  data_ov007_02102ddc.c = 0;
  data_ov007_02102ddc.d = 0;
  data_ov007_02102ddc.e = 0;
  data_ov007_02103474 = a2;
}
