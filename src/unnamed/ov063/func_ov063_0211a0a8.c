int func_ov063_0211a0a8(int a0, int a1, int a2, int a3, int a4) {
    int diff = a1 - a2;
    if (diff > 0) {
        if (diff < a3) return 0;
        return -a4;
    } else {
        a3 = -a3;
        if (diff > a3) return 0;
        return a4;
    }
}
