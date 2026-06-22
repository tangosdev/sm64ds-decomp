extern void func_02037e9c(char *p);
extern char data_020a0c78;

void func_020381cc(void *r0, int i, void **out)
{
    unsigned char *e = *(unsigned char **)r0;
    if (e == 0 || *(unsigned short *)(e + 4) != 8) {
        func_02037e9c(&data_020a0c78);
        *out = &data_020a0c78;
        return;
    }
    *out = e + 8 + (i << 3);
}
