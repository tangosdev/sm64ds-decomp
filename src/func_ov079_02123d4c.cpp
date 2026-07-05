//cpp
typedef short s16;
typedef long long s64;
extern s16 data_02082214[];

struct Obj {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v0a(); virtual void v0b();
    virtual void v0c(); virtual void v0d(); virtual void v0e(); virtual void v0f();
    virtual void v10(); virtual void v11(); virtual void v12(); virtual void v13();
    virtual void v14(); virtual void v15(); virtual void v16(); virtual void v17();
    virtual void v18(); virtual void v19(); virtual void v1a(); virtual void v1b();
    virtual void v1c(); virtual int  m1d();
};

extern "C" void func_ov079_02123d4c(int* out, char* a){
    out[0] = *(int*)(a + 0x5c);
    out[1] = *(int*)(a + 0x60);
    out[2] = *(int*)(a + 0x64);
    {
        int m;
        int d;
        if (*(unsigned char*)(a + 0x414)) {
            m = (s16)data_02082214[((unsigned short)*(unsigned short*)(a + 0x8c) >> 4) * 2] * (s16)0x190;
        } else {
            m = (s16)data_02082214[((unsigned short)*(unsigned short*)(a + 0x8c) >> 4) * 2] * (s16)0xc8;
        }
        out[0] = out[0] + (int)(((s64)m * data_02082214[((unsigned short)*(unsigned short*)(a + 0x8e) >> 4) * 2] + 0x800) >> 12);
        d = ((struct Obj*)a)->m1d();
        {
            int* p1 = (int*)(((int)out + 4) & 0xFFFFFFFFFFFFFFFF);
            int* p2 = (int*)(((int)out + 8) & 0xFFFFFFFFFFFFFFFF);
            *p1 = *p1 + d;
            *p2 = *p2 + (int)(((s64)m * data_02082214[((unsigned short)*(unsigned short*)(a + 0x8e) >> 4) * 2 + 1] + 0x800) >> 12);
        }
    }
}
