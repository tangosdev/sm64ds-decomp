typedef unsigned short u16;
typedef unsigned char u8;
typedef short s16;
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int x);
extern void func_02012790(int x);
extern void func_ov006_020f1ef8(void *c, int x);

void func_ov006_020f0eac(char *c)
{
    if (*(u16 *)(c + 0x5172) != 0) {
        {
            u8 *q = (u8 *)(((int)c + 0x51fb));
            *q = *q + 1;
        }
        if (*(u8 *)(c + 0x51fb) < 0x3c)
            return;
        *(u8 *)(c + 0x51fb) = 0;
        {
            u16 *p = (u16 *)(((int)c + 0x5172));
            *p = *p - 1;
        }
        if (*(s16 *)(c + 0x5172) <= 0)
            *(s16 *)(c + 0x5172) = 0;
        if (*(u16 *)(c + 0x5172) <= 2) {
            _ZN5Sound12PlayBank2_2DEj(0xa6);
            return;
        }
        _ZN5Sound12PlayBank2_2DEj(0xa7);
        return;
    }
    _ZN5Sound12PlayBank2_2DEj((unsigned int)((*(u8 *)(c + 0x545a) << 3) + 0xc0));
    func_02012790(0xe);
    func_ov006_020f1ef8(c, 0);
}
