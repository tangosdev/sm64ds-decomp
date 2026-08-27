//cpp
struct C;
typedef void (C::*PMF)(unsigned char*, int, int);

extern "C" {
extern PMF data_ov002_0210b708;
extern PMF data_ov002_0210b678;
extern PMF data_ov002_0210b618;
extern PMF data_ov002_0210b620;
extern PMF data_ov002_0210b640;
extern PMF data_ov002_0210b628;
extern PMF data_ov002_0210b648;
extern PMF data_ov002_0210b8c0;
extern PMF data_ov002_0210b900;
extern PMF data_ov002_0210b938;
extern PMF data_ov002_0210b930;
extern PMF data_ov002_0210b928;
extern PMF data_ov002_0210b920;
extern PMF data_ov002_0210b918;
extern PMF data_ov002_0210b910;
extern PMF data_ov002_0210b908;
extern PMF data_ov002_0210b750;
extern PMF data_ov002_0210b8f8;
extern PMF data_ov002_0210b8f0;
extern PMF data_ov002_0210b8e8;
extern PMF data_ov002_0210b8e0;
extern PMF data_ov002_0210b8d8;
extern PMF data_ov002_0210b8d0;
extern PMF data_ov002_0210b8c8;
}

extern "C" void func_ov002_020f7d74(C* self, unsigned char* p, int a2, int a3)
{
    static PMF data_ov002_0211104c[24] = {
        data_ov002_0210b708,
        data_ov002_0210b678,
        data_ov002_0210b618,
        data_ov002_0210b620,
        data_ov002_0210b640,
        data_ov002_0210b628,
        data_ov002_0210b648,
        data_ov002_0210b8c0,
        data_ov002_0210b900,
        data_ov002_0210b938,
        data_ov002_0210b930,
        data_ov002_0210b928,
        data_ov002_0210b920,
        data_ov002_0210b918,
        data_ov002_0210b910,
        data_ov002_0210b908,
        data_ov002_0210b750,
        data_ov002_0210b8f8,
        data_ov002_0210b8f0,
        data_ov002_0210b8e8,
        data_ov002_0210b8e0,
        data_ov002_0210b8d8,
        data_ov002_0210b8d0,
        data_ov002_0210b8c8,
    };
    (self->*data_ov002_0211104c[p[6]])(p + 7, a2, a3);
}
