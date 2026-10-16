extern int *dat_0380FFF4;
extern int fn_0238245C(void);
extern int fn_0238244C(void *);

int fn_023957C4(void)
{
    int result;
    if (fn_0238245C() != 0) {
        fn_0238244C((char *)dat_0380FFF4 + 0x5C0);
        result = fn_0238244C((char *)dat_0380FFF4 + 0x5EC);
    } else {
        unsigned short *p = (unsigned short *)dat_0380FFF4 + 0x181;
        result = *p | 0x40;
        *p = result;
    }
    return result;
}
