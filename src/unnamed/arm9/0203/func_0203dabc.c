struct E17 { char _f0[4]; int f4; char _rest[0x24 - 8]; };
extern struct E17 data_020a1154[];
int *func_0203dabc(int idx) {
    return &data_020a1154[idx].f4;
}
