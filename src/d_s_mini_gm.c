// @symbol dScMiniGm_c_classInit
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN7fBase_cC2Ev(void *);
extern int data_0208e4b8[];
extern int _ZTV8dScene_c[];
extern int data_ov005_020c2490[];
/* Reconstructed source-style name: SM64DS proves dScMiniGm_c through RTTI,
 * allocation size, vtable identity, and the MINIGAME registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: func_ov005_020c21ec. */
int *dScMiniGm_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(0xb0);
    if (p) {
        _ZN7fBase_cC2Ev(p);
        p[0] = (int)data_0208e4b8;
        p[0] = (int)_ZTV8dScene_c;
        *(unsigned char *)(((int)p + 0x13)) |= 1;
        *(unsigned char *)(((int)p + 0x13)) |= 4;
        p[0] = (int)data_ov005_020c2490;
    }
    return p;
}
