int func_0206ca44(unsigned p) {
    int count = 0;
    while (((p & 1) ? ((*(unsigned short *)(p - 1) & 0xff00) >> 8)
                    : (*(unsigned short *)p & 0xff)) != 0) {
        count++;
        p++;
    }
    return count;
}
