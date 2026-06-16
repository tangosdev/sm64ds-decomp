extern int _ZN9Animation8FinishedEv(void *);
extern int func_ov081_0212777c(void *, int);
extern void _ZN9Animation7AdvanceEv(void *);
extern int func_ov081_02126758(void *);
extern void _ZN12CylinderClsn5ClearEv(void *);
extern void _ZN12CylinderClsn6UpdateEv(void *);
int func_ov081_02127398(char *c) {
    if (_ZN9Animation8FinishedEv(c + 0x124)) {
        func_ov081_0212777c(c, *(int*)(c + 0x3e4));
    }
    _ZN9Animation7AdvanceEv(c + 0x124);
    func_ov081_02126758(c);
    _ZN12CylinderClsn5ClearEv(c + 0x1b0);
    _ZN12CylinderClsn6UpdateEv(c + 0x1b0);
    return 1;
}
