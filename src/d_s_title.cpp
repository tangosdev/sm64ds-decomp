//cpp
// @symbol dScTitle_c_classInit
extern "C" {
extern char data_0208e4b8[];
extern char _ZTV8dScene_c[];
extern char data_ov003_020b1650[];
void *_ZN7fBase_cnwEj(unsigned int);
void _ZN7fBase_cC2Ev(void *);
void *dScTitle_c_classInit(void);
}

/* Reconstructed source-style name: SM64DS proves dScTitle_c through RTTI,
 * allocation size, vtable identity, and the TITLE registry profile; later EAD
 * lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: func_ov003_020adc10. */
void *dScTitle_c_classInit(void) {
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
