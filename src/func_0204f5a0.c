typedef unsigned char u8;

struct Obj2;

extern void func_0204f3e0(void *c);
extern void func_02052494(struct Obj2 *o, int a, int b);
extern void func_0204f214(void *o, int a);

void func_0204f5a0(u8 *thiz, int arg1)
{
    if (thiz == 0)
        return;
    if (thiz[0x2c] == 0)
        return;
    if (arg1 == 0) {
        func_0204f3e0(thiz);
        return;
    }
    func_02052494((struct Obj2 *)(thiz + 0x1c), 0, arg1);
    func_0204f214(thiz, 0);
    thiz[0x2c] = 2;
}
