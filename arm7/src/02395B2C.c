extern int fn_023A4340(int, int, void *);
extern int fn_02395238(int, int);

int fn_02395B2C(void)
{
    int v4;
    int v0;
    unsigned int i;
    int v1;
    int result;
    v0 = 0x64;
    v1 = 0;
    v4 = 0;
    for (i = 0; i < 0x69; ++i) {
        fn_023A4340(v0, 1, &v4);
        if (i == 1)
            v4 &= 0x80;
        result = fn_02395238(v4 & 0xFF, v1);
        v1 = result;
        ++v0;
    }
    return result;
}
