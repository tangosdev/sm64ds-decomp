//cpp
struct C;
typedef void (C::*PMF)(unsigned char*, int, int);
extern PMF data_ov002_0210a14c;
extern PMF data_ov002_0210a124;
extern PMF data_ov002_0210a054;
extern PMF data_ov002_0210a0dc;
extern PMF data_ov002_0210a534;
extern PMF data_ov002_0210a0b4;
extern PMF data_ov002_0210a094;
extern PMF data_ov002_0210a40c;
extern PMF data_ov002_0210a474;
extern PMF data_ov002_0210a44c;
extern PMF data_ov002_0210a3fc;
extern PMF data_ov002_0210a3c4;
extern PMF data_ov002_0210a064;
extern PMF data_ov002_0210a36c;

extern "C" void func_ov002_020bd664(C* self, unsigned char* p, int a2, int a3) {
    static PMF table[14] = {
        data_ov002_0210a14c,
        data_ov002_0210a124,
        data_ov002_0210a054,
        data_ov002_0210a0dc,
        data_ov002_0210a534,
        data_ov002_0210a0b4,
        data_ov002_0210a094,
        data_ov002_0210a40c,
        data_ov002_0210a474,
        data_ov002_0210a44c,
        data_ov002_0210a3fc,
        data_ov002_0210a3c4,
        data_ov002_0210a064,
        data_ov002_0210a36c,
    };
    (self->*table[p[6]])(p + 7, a2, a3);
}
