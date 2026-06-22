typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

struct Elem { u8 pad0[4]; u16 f4; u8 pad6[2]; u8 f8; u8 f9; };

extern int *func_02050c14(u32 id);
extern void *func_020509b0(int x);
extern struct Elem *func_020523e0(void *obj, int idx);
extern int func_02051a98(void *a, int b, int c, int d, void *e, void *f, int g, int h);

int func_02051f1c(void *a0, u32 a1, int a2)
{
    int *r;
    void *obj;
    struct Elem *e;
    r = func_02050c14(a1);
    if (r == 0) return 0;
    obj = func_020509b0(r[0]);
    if (obj == 0) return 0;
    e = func_020523e0(obj, a2);
    if (e == 0) return 0;
    return func_02051a98(a0, e->f9, e->f4, e->f8, e, obj, (int)a1, a2);
}
