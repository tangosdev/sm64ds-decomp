extern void func_ov006_020c4048(void *);
extern void _ZN9ModelAnimC1Ev(void *);
extern int data_ov006_0213b0cc[];
int *func_ov006_020c8a04(int *t)
{
    func_ov006_020c4048(t);
    t[0] = (int)data_ov006_0213b0cc;
    _ZN9ModelAnimC1Ev((char *)t + 0x4c);
    return t;
}
