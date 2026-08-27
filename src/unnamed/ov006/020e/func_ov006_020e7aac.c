extern void func_020733a8(void *obj, int a, int b, void *cb1, void *cb2);
extern void _ZN9ModelAnimD1Ev(void);
extern void _ZN9ModelAnimC1Ev(void);
extern void _ZN15MaterialChangerD1Ev(void);
extern void _ZN15MaterialChangerC1Ev(void);

void *func_ov006_020e7aac(char *this)
{
    func_020733a8(this, 3, 0x64, (void *)_ZN9ModelAnimC1Ev, (void *)_ZN9ModelAnimD1Ev);
    func_020733a8(this + 0x12c, 3, 0x14, (void *)_ZN15MaterialChangerC1Ev, (void *)_ZN15MaterialChangerD1Ev);
    return this;
}
