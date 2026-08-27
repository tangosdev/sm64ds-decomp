struct Obj2 { int f0; int f4; int f8; int fc; };

int func_02052458(struct Obj2 *o, int a, int b);

void func_02052494(struct Obj2 *o, int a, int b) {
    o->f0 = func_02052458(o, a, b);
    o->f4 = a;
    o->fc = b;
    o->f8 = 0;
}
