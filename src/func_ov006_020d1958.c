//cpp
// NONMATCHING: register allocation (div=4). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" void _ZN5Sound12PlayBank2_2DEj(unsigned int);
extern unsigned char data_020a0e40;
extern unsigned char TOUCH_INPUT_ARR[];
extern unsigned char data_020a0de9[];
extern unsigned char data_020a0dea[];
extern unsigned char data_020a0deb[];

struct Base {
    virtual int v00(); virtual int v04(); virtual int v08(); virtual int v0c();
    virtual int v10(); virtual int v14(); virtual int v18(); virtual int v1c();
    virtual int v20(); virtual int v24(); virtual int v28(); virtual int v2c();
    virtual int v30(); virtual int v34(); virtual int v38(); virtual int v3c();
    virtual int v40(); virtual int v44(); virtual int v48(); virtual int v4c();
    virtual int v50(); virtual int v54(); virtual int v58(); virtual int v5c();
    virtual int v60(); virtual int v64(); virtual int v68(); virtual int v6c();
    virtual int v70(); virtual int v74(); virtual int v78(); virtual int v7c();
    virtual int v80(); virtual int v84(); virtual int v88(); virtual int v8c();
    virtual int v90();
};

extern "C" void func_ov006_020d1958(Base *o)
{
    char *c = (char *)o;
    if (o->v90() == 0) return;
    if (*(unsigned char *)(c + 0x5000 + 0x3dd) == 1) return;
    unsigned int i = data_020a0e40;
    int b3 = 0;
    if (TOUCH_INPUT_ARR[i << 2] != 0) {
        if (data_020a0de9[i << 2] != 0) b3 = 1;
    }
    if (b3 == 0) return;
    int v1 = data_020a0dea[i << 2];
    int v0 = data_020a0deb[i << 2];
    if (v1 < 0x60) return;
    if (v1 >= 0xa0) return;
    if (v0 < 0xa0) return;
    if (v0 >= 0xc0) return;
    *(unsigned char *)(c + 0x5000 + 0x3dd) = 1;
    _ZN5Sound12PlayBank2_2DEj(0x151);
}
