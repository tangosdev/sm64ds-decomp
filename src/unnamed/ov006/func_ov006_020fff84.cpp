//cpp
struct C; typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };
extern Entry data_ov006_021426cc[];
struct C { char pad[1]; };
extern "C" void func_ov006_020fff84(C *c);
#pragma opt_strength_reduction off
void func_ov006_020fff84(C *c) {
    int i;
    for (i = 0; i < 2; i++) {
        char *b = (char *)c + (i << 5);
        if (*(unsigned char *)(b + 0x5634) != 0) {
            (c->*data_ov006_021426cc[*(unsigned char *)(b + 0x5635)].pmf)(i);
        }
    }
}
