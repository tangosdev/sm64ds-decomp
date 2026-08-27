//cpp
extern "C" {
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *clsn);
extern void func_02038414(void *p);
extern int _ZNK10dBgCh_Actr13JustHitGroundEv(void *self);
extern int DecIfAbove0_Byte(void *p);
extern void func_ov096_0213585c(void *t);
int func_ov096_021360c4(char *c) {
    int eq = (*(unsigned short*)(c+0xc) == 0xf0) ? 1 : 0;
    if (eq != 0) {
        *(short*)(c+0x8c) = *(short*)(c+0x8c) - 0x1000;
    }
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c+0x14c);
    func_02038414(c+0x180);
    if (_ZNK10dBgCh_Actr13JustHitGroundEv(c+0x180) || !DecIfAbove0_Byte(c+0x3ac)) {
        func_ov096_0213585c(c);
    }
    return 1;
}
}
