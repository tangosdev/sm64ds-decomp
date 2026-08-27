//cpp
extern "C" {
    extern void func_ov062_02117994(void *, int);
    extern void func_ov062_021175c0(void *);
}

struct dBgCh_Actr {
    int IsOnGround() const;
};

extern "C" void func_ov062_02118a00(void *c) {
    int gr = ((dBgCh_Actr *)((char *)c + 0x144))->IsOnGround();
    if (gr == 0) return;
    int v = *(int *)((char *)c + 0x390);
    if (v == 1) {
        *(int *)((char *)c + 0x38c) = 5;
    } else {
        *(int *)((char *)c + 0x38c) = 2;
    }
    func_ov062_02117994(c, 0);
    func_ov062_021175c0(c);
}
