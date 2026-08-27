extern void MultiCopyHalf(int src, char *dst, int nbytes);
extern void MultiStore16(unsigned short val, char *dst, int nbytes);
struct S { int f0; char buf[1]; };
void func_ov007_020c940c(struct S *s) {
    if (s->f0 != 0) {
        MultiCopyHalf(s->f0, (char*)s + 4, 0x40);
    } else {
        volatile unsigned short tmp = 0;
        MultiStore16(tmp, (char*)s + 4, 0x40);
    }
}
