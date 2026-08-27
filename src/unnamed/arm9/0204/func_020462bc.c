struct E { char _pad[0x30]; };
struct E *func_020462bc(int *p) {
    return &((struct E *)0)[p[1]];
}
