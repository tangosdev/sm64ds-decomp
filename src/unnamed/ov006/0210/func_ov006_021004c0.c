struct Obj { char pad[0x5660]; int f; /* 0x5660 */ };

extern int func_ov004_020adc1c(void);
extern void func_ov004_020b19f0(void);

void func_ov006_021004c0(struct Obj *o)
{
    if (o->f < 2)
        return;
    func_ov004_020adc1c();
    func_ov004_020b19f0();
}
