int func_02071708(int w0, ...)
{
    unsigned char *p;
    int count;
    unsigned int c;
    if (w0 != 0) {
        p = (unsigned char *)&w0;
        count = 0;
        c = *p;
        if (c == 0) {
            do {
                ++p;
                count += 8;
                c = *p;
            } while (c == 0);
        }
        c = ~c;
        *p = (unsigned char)c;
        if ((*(volatile unsigned char *)p & 1) == 0)
            return count;
        do {
            count++;
            *p = (unsigned char)((int)*(volatile unsigned char *)p >> 1);
        } while (*(volatile unsigned char *)p & 1);
        return count;
    }
    return 0x20;
}
