void MultiStore16(unsigned short val, char *dst, int nbytes) {
    int i = 0;
    do {
        if (i < nbytes) {
            *(unsigned short*)(dst + i) = val;
            i += 2;
        } else break;
    } while (1);
}
