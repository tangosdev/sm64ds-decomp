//cpp
// @symbol dScGameOver_c_classInit
extern "C" {
extern void *data_0208e4b8;
extern void *_ZTV8dScene_c;
extern void *data_ov003_020b179c;
void *_ZN7fBase_cnwEj(unsigned int);
void _ZN7fBase_cC2Ev(void *);
void *dScGameOver_c_classInit(void);
}

/* Reconstructed source-style name: SM64DS proves dScGameOver_c through RTTI,
 * allocation size, vtable identity, and the GAME_OVER registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: func_ov003_020b1118. */
void *dScGameOver_c_classInit(void) {
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
