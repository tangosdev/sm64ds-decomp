//cpp
struct VObj {
    virtual void f00();
    virtual void f01();
    virtual void f02();
    virtual void f03();
    virtual void f04();
    virtual void f05();
    virtual void f06();
    virtual void f07();
    virtual void f08();
    virtual void f09();
    virtual void f10();
    virtual void f11();
    virtual void f12();
    virtual void f13();
    virtual void f14();
    virtual void f15();
    virtual void f16();
    virtual void f17();
    virtual int IsTargetable();
};

extern "C" {
extern char* _ZN8dActor_c4NextEPKS_(const char* a);
extern int Vec3_Dist(const void* a, const void* b);
extern short Vec3_HorzAngle(const void* a, const void* b);
extern short Vec3_VertAngle(const void* a, const void* b);
extern char* func_ov002_020d708c(char* self);
}

char* func_ov002_020d708c(char* self) {
    char* a;
    char* best = 0;
    int bestAng = 0x238e;
    a = _ZN8dActor_c4NextEPKS_(best);
    if (a) {
        int thresh = bestAng;
        do {
            if (a != self) {
                if (((VObj*)a)->IsTargetable()) {
                    if (Vec3_Dist(self + 0x5c, a + 0x5c) < 0x15e000) {
                        int diff = (short)(*(short*)(self + 0x8e) - Vec3_HorzAngle(self + 0x5c, a + 0x5c));
                        int ad = diff < 0 ? -diff : diff;
                        if (ad <= thresh) {
                            int ad2 = diff < 0 ? (short)(-diff) : diff;
                            if (ad2 < bestAng) {
                                bestAng = diff;
                                best = a;
                            }
                        }
                    }
                }
            }
            a = _ZN8dActor_c4NextEPKS_(a);
        } while (a);
    }
    if (best) {
        short v = Vec3_VertAngle(self + 0x5c, best + 0x5c);
        *(short*)(self + 0x762) = 0;
        *(short*)(self + 0x764) = (short)bestAng;
        *(short*)(self + 0x766) = v;
        *(unsigned char*)(self + 0x742) = 1;
    }
    return best;
}
