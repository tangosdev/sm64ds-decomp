/* One byte through the aligned u16 accesses this debug-output library uses. */
static inline int ReadByte(const void *a) {
    unsigned p = (unsigned)a;
    if (p & 1)
        return (*(unsigned short *)(p - 1) & 0xff00) >> 8;
    return *(unsigned short *)p & 0xff;
}

/* strlen for func_0206a928's %s. */
int func_0206ca44(const char *s) {
    unsigned p = (unsigned)s;
    int count = 0;
    while (ReadByte((const void *)p) != 0) {
        count++;
        p++;
    }
    return count;
}
