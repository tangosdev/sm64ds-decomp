/* strlen through the cluster's u16-only byte reads (no ldrb), for
   func_0206a928's %s. */
int func_0206ca44(const char *s) {
    unsigned p = (unsigned)s;
    int count = 0;
    while (((p & 1) ? ((*(unsigned short *)(p - 1) & 0xff00) >> 8)
                    : (*(unsigned short *)p & 0xff)) != 0) {
        count++;
        p++;
    }
    return count;
}
