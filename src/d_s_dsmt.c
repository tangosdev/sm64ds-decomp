// @symbol dScDSMT_c_classInit
// dScDSMT_c factory: new fBase_c(0x64), base ctor, installs the vtable chain
// (two base vtables then the ov007 one), ORs flag bits 1 and 4 at +0x13 via a
// materialized byte pointer, sets the +0x50 sub-object vtables, and constructs
// the member at +0x54 via func_02017278.
extern void *_ZN7fBase_cnwEj(unsigned int size);
extern void _ZN7fBase_cC2Ev(void *p);
extern void func_02017278(int *p);

extern int data_0208e4b8[];
extern int _ZTV8dScene_c[];
extern int data_ov007_021032e8[];
extern int data_0208ee14[];
extern int data_ov007_021032b0[];

/* Reconstructed source-style name: SM64DS proves dScDSMT_c through RTTI,
 * allocation size, vtable identity, and the DSMT registry profile; later EAD
 * lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: func_ov007_020ccad0. */
int *dScDSMT_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(0x64);
    if (p) {
        unsigned char *f;
        _ZN7fBase_cC2Ev(p);
        p[0] = (int)data_0208e4b8;
        p[0] = (int)_ZTV8dScene_c;
        f = (unsigned char *)((char *)p + 0x13);
        *f |= 1;
        *f |= 4;
        p[0] = (int)data_ov007_021032e8;
        p[0x50 / 4] = (int)data_0208ee14;
        p[0x50 / 4] = (int)data_ov007_021032b0;
        func_02017278(p + 0x54 / 4);
    }
    return p;
}
