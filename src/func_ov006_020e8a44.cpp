//cpp
typedef unsigned char u8;
class C;
typedef void (C::*PMF)(int);
class C { public: int dummy; };
struct Row { u8 d[0x20]; };

extern "C" PMF data_ov006_02141f1c[];

extern "C" void func_ov006_020e8a44(C *self)
{
    Row *rows = (Row *)self;
    for (int i = 0; i < 0x14; i++) {
        if (rows[i].d[0x52d4]) {
            u8 k = rows[i].d[0x52d9];
            (self->*data_ov006_02141f1c[k])(i);
        }
    }
}
