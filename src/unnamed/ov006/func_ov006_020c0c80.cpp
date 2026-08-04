//cpp
extern "C" {
extern void _ZN5Sound12PlayBank2_2DEj(unsigned);
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *, void *, int, int, int, unsigned short);
struct G2 { int w[2]; };
extern struct G2 data_ov006_0213ac80;
void func_ov006_020c0c80(void *c)
{
    char *r4 = (char *)c;
    _ZN5Sound12PlayBank2_2DEj(0x13b);
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(r4 + 0x1c, *(void **)(r4 + 0x244), 0, 0x40000000, 0x800, 0);
    *(short *)(r4 + 0x1d8) = 5;
    *(struct G2 *)r4 = data_ov006_0213ac80;
}
}
