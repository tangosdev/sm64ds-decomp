//cpp
struct C; typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };
extern Entry data_ov006_02142254[];
struct C { char pad[1]; };
extern "C" void func_ov006_020f1e90(C *c);
void func_ov006_020f1e90(C *c) {
    int i;
    char *base = (char *)c;
    for (i = 0; i < 0x78; i++) {
        char *b = base + i;
        if (*(unsigned char *)(b + 0x52ed) != 0) {
            unsigned char k = *(unsigned char *)(b + 0x51fd);
            (c->*data_ov006_02142254[k].pmf)(i);
        }
    }
}
