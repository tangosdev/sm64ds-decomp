int func_ov002_020f02c8(int x) {
    switch (x) {
    case 4:
    case 5:
        return 0xa000;
    case 3:
        return 0x8000;
    case 0:
    default:
        return 0x7000;
    case 1:
    case 2:
        return 0x5000;
    }
}
