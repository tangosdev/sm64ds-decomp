//cpp
typedef unsigned char u8;
class C;
typedef void (C::*PMF)(int);
class C { public: int dummy; };
struct Row { u8 d[0x10]; };

extern "C" PMF data_ov006_02141680[];

extern "C" void func_ov006_020d65c8(C *self)
{
    Row *rows = (Row *)self;
    for (int i = 0; i < 2; i++) {
        if (rows[i].d[0x626d]) {
            u8 k = rows[i].d[0x626c];
            (self->*data_ov006_02141680[k])(i);
        }
    }
}
