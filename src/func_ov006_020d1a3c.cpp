//cpp
typedef unsigned char u8;

struct Obj {
    virtual int m00(); virtual int m01(); virtual int m02(); virtual int m03();
    virtual int m04(); virtual int m05(); virtual int m06(); virtual int m07();
    virtual int m08(); virtual int m09(); virtual int m10(); virtual int m11();
    virtual int m12(); virtual int m13(); virtual int m14(); virtual int m15();
    virtual int m16(); virtual int m17(); virtual int m18(); virtual int m19();
    virtual int m20(); virtual int m21(); virtual int m22(); virtual int m23();
    virtual int m24(); virtual int m25(); virtual int m26(); virtual int m27();
    virtual int m28(); virtual int m29(); virtual int m30(); virtual int m31();
    virtual int m32(); virtual int m33(); virtual int m34(); virtual int m35();
    virtual int m36();
};

extern "C" {
    extern u8 data_020a0e40;
    extern u8 data_020a0de8[];
    extern u8 data_020a0de9[];
    extern u8 data_020a0dea[];
    extern u8 data_020a0deb[];
}

extern "C" void func_ov006_020d1a3c(void *p)
{
    int idx;
    int flag;
    int a;
    int b;
    char *base;

    if (((Obj *)p)->m36()) return;

    base = (char *)p;
    idx = data_020a0e40;
    flag = 0;
    if (data_020a0de8[idx * 4]) {
        if (data_020a0de9[idx * 4] != 0) flag = 1;
    }
    if (flag != 0) {
        *(u8 *)(base + 0x53de) = 0;
        *(int *)(base + 0x53d8) = 0;
    }
    idx = data_020a0e40;
    if (data_020a0de8[idx * 4] != 0) {
        a = data_020a0dea[idx * 4];
        b = data_020a0deb[idx * 4];
        if ((a >= 0 && a < 0x10 && b >= 0x40 && b < 0x80) ||
            (a >= 0xf0 && a < 0x100 && b >= 0x40 && b < 0x80)) {
            if (*(int *)(base + 0x53d8) >= 5) {
                *(u8 *)(base + 0x53dc) = 1;
            } else {
                *(int *)(((long long)(int)(base + 0x53d8)) & 0xFFFFFFFFFFFFFFFFLL) += 1;
                *(u8 *)(base + 0x53dc) = 0;
            }
            *(u8 *)(base + 0x53de) = 1;
        } else {
            *(u8 *)(base + 0x53dc) = 0;
            *(u8 *)(base + 0x53de) = 0;
            *(int *)(base + 0x53d8) = 0;
        }
    } else {
        *(u8 *)(base + 0x53dc) = 0;
        *(int *)(base + 0x53d8) = 0;
    }
}
