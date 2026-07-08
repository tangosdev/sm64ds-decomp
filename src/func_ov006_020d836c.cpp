//cpp
typedef unsigned char u8;
struct C;
typedef void (C::*PMF)(int);
extern "C" {
extern u8 data_020a0e40;
extern int TOUCH_INPUT_ARR[];
void func_ov006_020d69b8(char *c, int i);
void func_ov006_020d836c(char *c);
}
extern PMF data_ov006_02141730[];
#pragma opt_strength_reduction off
void func_ov006_020d836c(char *c)
{
    if (*(u8*)(TOUCH_INPUT_ARR + data_020a0e40) == 0)
        *(u8*)(c + 0x62f6) = 0xff;
    int i;
    for (i = 0; i < 0x70; i++) {
        u8 *o = (u8*)c + i*0x40;
        u8 flag;
        o += 0x4000;
        flag = o[0x698];
        if (flag != 0) {
            C *cc = (C*)c;
            (cc->*data_ov006_02141730[o[0x697]])(i);
            func_ov006_020d69b8(c, i);
        }
    }
}
