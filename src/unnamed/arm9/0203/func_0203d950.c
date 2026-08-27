struct E { unsigned short flags; unsigned char pad[0x22]; };
extern struct E data_020a1160[];
int func_0203d950(int i) {
    return (data_020a1160[i].flags & 0x8000) != 0;
}
