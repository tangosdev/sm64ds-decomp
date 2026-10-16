/* storage 0x02395400 runtime 0x027C7378 size 0x10 main. Pool +0x10: 027C738C. */

extern void fn_02395414(int, int, unsigned int);

void fn_02395400(int dst, int src, int count) {
    fn_02395414(dst, src, (unsigned int)(count + 1) >> 1);
}
