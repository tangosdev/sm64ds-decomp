//cpp
extern "C" {
extern void *data_0208e4b8;
extern void *_ZTV8dScene_c;
extern void *data_ov003_020b179c;
void *_ZN7fBase_cnwEj(unsigned int);
void _ZN7fBase_cC2Ev(void *);
void *func_ov003_020b1118(void);
}

void *func_ov003_020b1118(void) {
    char *p = (char *)_ZN7fBase_cnwEj(0x98);
    if (p) {
        _ZN7fBase_cC2Ev(p);
        *(void **)p = &data_0208e4b8;
        *(void **)p = &_ZTV8dScene_c;
        *(unsigned char *)(int)(((int)p + 0x13)) |= 1;
        *(unsigned char *)(int)(((int)p + 0x13)) |= 4;
        *(void **)p = &data_ov003_020b179c;
    }
    return p;
}
