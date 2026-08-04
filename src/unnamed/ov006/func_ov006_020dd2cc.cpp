//cpp
struct C; typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };
extern Entry data_ov006_021417b0[];
struct E { char pad[0x18]; };
struct C { char pad[0x18]; };
extern "C" void func_ov006_020dd2cc(C *c);
void func_ov006_020dd2cc(C *c) {
    int i;
    struct E *arr = (struct E *)c;
    for (i = 0; i < 0x18; i++) {
        char *base = (char *)&arr[i];
        unsigned char k = *(unsigned char *)(base + 0x4ad0);
        (c->*data_ov006_021417b0[k].pmf)(i);
    }
}
