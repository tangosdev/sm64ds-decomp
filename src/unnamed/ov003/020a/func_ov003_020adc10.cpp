//cpp
extern "C" {
extern char data_0208e4b8[];
extern char _ZTV8dScene_c[];
extern char data_ov003_020b1650[];
void *_ZN7fBase_cnwEj(unsigned int);
void _ZN7fBase_cC2Ev(void *);
void *func_ov003_020adc10(void);
}

void *func_ov003_020adc10(void) {
    char *p = (char *)_ZN7fBase_cnwEj(0x54);
    if (p) {
        _ZN7fBase_cC2Ev(p);
        *(void **)p = (void *)data_0208e4b8;
        *(void **)p = (void *)_ZTV8dScene_c;
        unsigned char *bytePtr = (unsigned char *)(int)((unsigned long long)((int)p + 0x13));
        *bytePtr |= 1;
        *bytePtr |= 4;
        *(void **)p = (void *)data_ov003_020b1650;
    }
    return p;
}
