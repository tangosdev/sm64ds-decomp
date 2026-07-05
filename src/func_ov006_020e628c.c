extern void func_ov006_020e4b00(void*);
extern void func_ov006_020e5ffc(void*);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);
typedef struct Elem {
  int a;                /* 0x0 */
  int b;                /* 0x4 */
  short cc;             /* 0x8 */
  short d;              /* 0xa */
  unsigned char e;      /* 0xc */
  unsigned char f;      /* 0xd */
  char pad[2];          /* 0xe */
} Elem;
typedef struct Big {
  char pad0[0x4870];
  Elem arr[5];          /* 0x4870 */
  char pad1[0x5580 - 0x48c0];
  int st;               /* 0x5580 */
  char pad2[0x55b6 - 0x5584];
  unsigned short f;     /* 0x55b6 */
  char pad3[2];         /* 0x55b8 */
  unsigned char cnt;    /* 0x55ba */
  char pad4;            /* 0x55bb */
  unsigned char flag;   /* 0x55bc */
} Big;
#pragma opt_strength_reduction off
void func_ov006_020e628c(char *c){
  Big *t = (Big *)c;
  int i;
  func_ov006_020e4b00(c);
  if (t->f != 0){
    *(unsigned short *)(((int)c + 0x55b6) & 0xFFFFFFFFFFFFFFFF) -= 1;
    if ((short)t->f <= 0) t->f = 0;
    return;
  }
  t->st = 1;
  if (t->cnt >= 5){
    t->st = 3;
    t->f = 0x40;
    t->flag = 1;
    _ZN5Sound12PlayBank2_2DEj(0x1d8);
  }
  for (i = 0; i < 5; i++){
    t->arr[i].a = 0;
    t->arr[i].b = 0;
    t->arr[i].cc = 0;
    t->arr[i].d = 0;
    t->arr[i].f = 0;
    t->arr[i].e = 0;
  }
  func_ov006_020e5ffc(c);
}
