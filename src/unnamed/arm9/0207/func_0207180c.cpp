unsigned char *func_02071a50(unsigned char *p, int *out);

/* EH runtime: skip the header of an exception action table and return the
   first range record. The header is a flag byte, one more byte, then an
   unsigned varint (func_02071a50), and a second varint when flag 0x40 is
   set. func_02072dac calls it before walking the range records. */
unsigned char *func_0207180c(unsigned char *p) {
    int skipped;
    unsigned char flags = *p;
    p += 2;
    p = func_02071a50(p, &skipped);
    if (flags & 0x40) {
        p = func_02071a50(p, &skipped);
    }
    return p;
}
