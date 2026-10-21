extern void *_ZN6Memory13operator_new2Ej(unsigned int sz);

int *func_ov002_020e6bb0(char *self) {
    char **pp;
    char *base;
    unsigned int count;
    int *out;
    int *p;
    char *src;
    unsigned int i;

    pp = (char **)(((long long)(int)(self + 8)));
    base = *pp;
    count = *(unsigned int *)(base + 0x24);
    out = (int *)_ZN6Memory13operator_new2Ej(count << 2);

    p = out;
    src = pp[1];
    for (i = 0; i < count; i++) {
        *p++ = *(int *)(src + 0x1c);
        src += 0x30;
    }
    return out;
}

void func_ov002_020e6b74(char *c, int *src) {
    char *base = (char *)(c + 8);
    void *obj = *(void **)base;
    unsigned int n = *(unsigned int *)((char *)obj + 0x24);
    char *dst = *(char **)(base + 4);
    unsigned int i;
    for (i = 0; i < n; i++) {
        *(int *)(dst + 0x1c) = *src;
        dst += 0x30;
        src += 1;
    }
}

struct A {
    char pad[0x24];
    unsigned int count;
};

struct B {
    char pad[0x1c];
    unsigned int idx;
    char pad2[0x10];
};

void func_ov002_020e6b3c(char *o) {
    void **p = (void **)(o + 8);
    struct A *a = (struct A *)p[0];
    unsigned int n = a->count;
    struct B *b = (struct B *)p[1];
    unsigned int i;
    for (i = 0; i < n; i++, b++)
        b->idx = 0;
}

unsigned short *func_ov002_020e6ad4(unsigned short *p) {
    unsigned short *dst = p;
    unsigned short *q = (unsigned short *)((char *)p + 0x40);
    int i = 0;
    do {
        unsigned short v = (i * 0x400) | (i | (i * 0x20));
        *dst++ = v;
        *q++ = v;
        i++;
    } while (i < 0x20);
    *(volatile unsigned short *)0x4000060 &= ~0x3002;
    return p;
}

extern unsigned short Color_Interp(void* a, unsigned short x, unsigned short y, int z);
extern void func_020555a4(void* p);
extern unsigned short data_ov002_0210a760[];
void func_ov002_020e6a78(char* c, int b){
  unsigned short* src=(unsigned short*)c;
  unsigned short* tbl=data_ov002_0210a760;
  unsigned short* dst=(unsigned short*)(c+0x40);
  int i;
  for(i=0;i<0x20;i++){
    *dst++ = Color_Interp(c, *src++, *tbl++, b);
  }
  func_020555a4(c+0x40);
}
