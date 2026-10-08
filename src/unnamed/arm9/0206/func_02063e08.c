int func_02065970(void);
int func_02065bc0(void);
int func_02065bd0(void);
int func_02064eac(int p0, unsigned short mask, unsigned char e2, short a, int b, int c);

int func_02063e08(char* obj, int idx, int p2)
{
    unsigned char* e;
    unsigned short mask;
    int a, b, c, r;

    r = 0;
    e = (unsigned char*)(obj + 0x1d4 + idx * 0x68);
    mask = (unsigned short)(1 << idx);
    if (e[0] == 2 && e[1] == 2) {
        a = func_02065970();
        b = func_02065bc0();
        c = func_02065bd0();
        r = func_02064eac(p2, mask, e[2], (short)a, b, c);
        e[0] = 1;
    }
    return r;
}
