/* storage 0x0239707C runtime 0x027C8FF4 size 0x4C main. */

extern void fn_02396FE8(void);

int fn_0239707C(unsigned short *arg) {
    char **pp;
    volatile unsigned short *src;

    src = arg;
    pp = (char **)0x0380FFF4;
    *(unsigned short *)(*pp + 0x300 + 0x5C) = src[0];
    *(unsigned short *)(*pp + 0x300 + 0x5E) = (unsigned short)(src[1] | src[0]);
    fn_02396FE8();
    return 0;
}
