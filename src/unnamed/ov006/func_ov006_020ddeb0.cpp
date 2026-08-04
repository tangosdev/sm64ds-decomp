//cpp
extern "C" int RandomIntInternal(int *seed);
extern "C" int data_0209d4b8;

struct Obj {
    virtual int v00(); virtual int v01(); virtual int v02(); virtual int v03();
    virtual int v04(); virtual int v05(); virtual int v06(); virtual int v07();
    virtual int v08(); virtual int v09(); virtual int v10(); virtual int v11();
    virtual int v12(); virtual int v13(); virtual int v14(); virtual int v15();
    virtual int v16(); virtual int v17(); virtual int v18(); virtual int v19();
    virtual int v20(); virtual int v21(); virtual int v22(); virtual int v23();
    virtual int v24(); virtual int v25(); virtual int v26(); virtual int v27();
    virtual int v28(); virtual int v29(); virtual int v30(); virtual int v31();
    virtual int v32(); virtual int v33(); virtual int v34();
    virtual int m();
};

extern "C" void func_ov006_020ddeb0(Obj *thiz)
{
    int n;
    unsigned int r8 = 0;
    int i;
    char *c = (char *)thiz;
    n = thiz->m() != 0 ? 0x10 : 0x28;
    for (i = 0; i < n; i++) {
        unsigned int v = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        *(int *)(c + 0x4660) = ((((v << 3) >> 0xf) << 5) + 0x10) << 0xc;
        *(int *)(c + 0x4664) = -0xf0000;
        *(unsigned char *)(c + 0x4677) = 1;
        *(unsigned char *)(c + 0x4676) = 1;
        {
            unsigned int w = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            *(short *)(c + 0x4670) = (short)(((i & 0xf) << 3) + (((w << 4) >> 0xf) << 4));
        }
        *(unsigned char *)(c + 0x4674) = (unsigned char)(r8 + 3);
        r8 = (r8 + 1) & 0xff;
        if (r8 >= 3) r8 = 0;
        c += 0x1c;
    }
}
