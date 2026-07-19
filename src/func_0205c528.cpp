//cpp
extern "C" {
    int _ZN3IRQ7DisableEv(void);
    void _ZN3IRQ7RestoreEj(unsigned int);
    void func_020580f0(void *p);
}

struct Obj;
typedef int (*M44)(Obj *self, int a, int b, int c);
struct Obj {
    int f0, f4, f8, fc, f10;
    int pad[12];
    M44 m44;
};
struct Outer {
    Obj *obj;
    int f4;
};

extern "C" void func_0205c528(Outer *thiz, int r1, int r7)
{
    Obj *o = thiz->obj;
    *(int *)(((int)o + 0x10) & 0xFFFFFFFFFFFFFFFFULL) |= 0x200;
    int res = o->m44(o, r1, thiz->f4, r7);
    switch (res) {
    case 0:
    case 1:
        *(int *)(((long)o + 0x10) & 0xFFFFFFFFFFFFFFFFULL) &= ~0x200;
        break;
    case 6: {
        unsigned int saved = (unsigned int)_ZN3IRQ7DisableEv();
        while ((o->f10 & 0x200) ? 1 : 0) {
            func_020580f0(&o->fc);
        }
        _ZN3IRQ7RestoreEj(saved);
        break;
    }
    }
    *(int *)(((int)thiz + 4) & 0xFFFFFFFFFFFFFFFFULL) += r7;
}
