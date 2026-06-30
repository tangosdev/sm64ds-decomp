//cpp
typedef unsigned char u8;
class C;
typedef void (C::*PMF)(int);
class C { public: int dummy; };
struct Row { u8 d[0x40]; };
extern "C" void func_ov006_0210076c(C *self, int i);
extern "C" PMF data_ov006_02142734[];
extern "C" void func_ov006_0210246c(C *self)
{
    Row *rows = (Row *)self;
    for (int i = 0; i < 3; i++) {
        if (rows[i].d[0x5294]) {
            (self->*data_ov006_02142734[rows[i].d[0x5296]])(i);
            func_ov006_0210076c(self, i);
        }
    }
}
