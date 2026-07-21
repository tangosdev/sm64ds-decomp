typedef unsigned short u16;
typedef unsigned char u8;
extern void func_ov006_020dc26c(char *c);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);
extern u16 data_ov006_0212e33c[];
extern u8 data_ov006_0212e310[];
void func_ov006_020dc5c4(char *c, int i)
{
    int off = i * 0x14;
    u16 *counter = (u16 *)(c + 0x519e + off);
    u8  *st = (u8 *)(c + 0x51a4 + off);
    *counter = (u16)(*counter + 1);
    if (*counter < data_ov006_0212e33c[*st])
        return;
    *counter = 0;
    *st = (u8)(*st + 1);
    if (*st == 2) {
        func_ov006_020dc26c(c);
        _ZN5Sound12PlayBank2_2DEj(0xf0);
    }
    if (*st >= 4) {
        *(u16 *)(c + 0x519c + off) = 0xb4;
        *(u8 *)(c + 0x51a1 + off) = 2;
        {
            int *p = (int *)(((int)c + 0x51c8) & 0xFFFFFFFFFFFFFFFF);
            *st = 0;
            *counter = 0;
            *p = *p + 1;
        }
    } else {
        *(u8 *)(c + 0x51a3 + off) = data_ov006_0212e310[*st];
    }
}
