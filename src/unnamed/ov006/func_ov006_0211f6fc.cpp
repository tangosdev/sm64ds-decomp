//cpp
struct C;
typedef void (C::*PMF)(int);
extern "C" void func_ov006_0211f664(C *this_, int i);
extern PMF data_ov006_02142ed8[];
struct C { };
extern "C" void func_ov006_0211f6fc(C *c) {
    int i;
    char *e = (char*)c;
    for (i = 0; i < 0x10; i++, e += 0x24) {
        if (*(unsigned char*)(e + 0x4677)) {
            (c->*data_ov006_02142ed8[*(unsigned char*)(e + 0x4678)])(i);
            func_ov006_0211f664(c, i);
        }
    }
}
