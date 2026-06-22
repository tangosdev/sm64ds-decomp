void MultiStore16(short val, char *dst, int nbytes) {
    int i = 0;
    do {
        if (i < nbytes) {
            *(short*)(dst + i) = val;
            i += 2;
        } else break;
    } while (1);
}
