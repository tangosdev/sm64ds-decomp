extern void fn_0238D1FC(unsigned short *);

int fn_0238C420(void) {
    unsigned short value;
    fn_0238D1FC(&value);
    if (value & 1)
        return 0;
    return (value & 2) != 0;
}
