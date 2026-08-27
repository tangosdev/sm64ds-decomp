//cpp
struct Obj {
    virtual void p00(); virtual void p01(); virtual void p02(); virtual void p03();
    virtual void p04(); virtual void p05(); virtual void p06(); virtual void p07();
    virtual void p08(); virtual void p09(); virtual void p10(); virtual void p11();
    virtual void p12(); virtual void p13(); virtual void p14(); virtual void p15();
    virtual void p16(); virtual void p17(); virtual void p18(); virtual void p19();
    virtual void p20(); virtual void p21(); virtual void p22(); virtual void p23();
    virtual void p24(); virtual void p25(); virtual void p26(); virtual void p27();
    virtual void p28(); virtual void p29(); virtual void p30(); virtual void p31();
    virtual void p32(); virtual void p33(); virtual void p34();
    virtual int check();
};

extern int data_ov006_0212ef7c[];
extern int data_ov006_0212ef8c[];

extern "C" void func_ov006_0211b9c8(Obj *obj) {
    int lr;
    int ip;
    int found;
    int off;
    if (obj->check()) {
        lr = data_ov006_0212ef7c[*(unsigned char*)((char*)obj + 0x5627)];
    } else {
        lr = data_ov006_0212ef8c[*(unsigned char*)((char*)obj + 0x5627)];
    }
    for (ip = 0, off = 0; ip < 2; ip++) {
        int sb;
        found = -1;
        for (sb = 0; sb < lr; sb++) {
            int e = sb + off;
            if (*(unsigned char*)((char*)obj + e * 20 + 0x50f5) != 0)
                found = e;
        }
        if (found != -1) {
            *(unsigned char*)((char*)obj + found * 20 + 0x50f4) = 4;
        }
        off += lr;
    }
}
