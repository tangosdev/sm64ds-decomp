extern unsigned SDK_WRAM_ARENA_LO;

int fn_023818E8(int a1) {
    unsigned value;
    switch (a1) {
    case 1:
        return 0x027D80E0;
    case 7:
        value = (unsigned)&SDK_WRAM_ARENA_LO;
        if (value > 0x03800000)
            return 0x03800000;
        return (int)value;
    case 8:
        value = 0x03800000;
        if ((unsigned)&SDK_WRAM_ARENA_LO > 0x03800000)
            value = (unsigned)&SDK_WRAM_ARENA_LO;
        return (int)value;
    default:
        return 0;
    }
}
