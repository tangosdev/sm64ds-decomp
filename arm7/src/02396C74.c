/* storage 0x02396C74 runtime 0x027C8BEC size 0x4C main. */

extern void fn_02382C98(unsigned char *src, unsigned int dst, unsigned int count);

int fn_02396C74(unsigned int count, unsigned char *src) {
    if (count > 0x80)
        return 4;
    fn_02382C98(src, *(unsigned int *)(*(char **)0x0380FFF4 + 0x398), count + 1);
    *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0x9C) = (unsigned short)count;
    return 0;
}
