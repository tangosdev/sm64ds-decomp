extern int _Z14ApproachLinearRiii(int *, int, int);
extern int _ZN9Animation8FinishedEv(void *);
extern void func_ov062_02117994(void *, int, int);

void func_ov062_021180d4(void *c) {
    _Z14ApproachLinearRiii((int*)((char*)c + 0x98), 0, 0x1000);
    int done = _ZN9Animation8FinishedEv((char*)c + 0x350);
    if (!done) return;
    *(int*)((char*)c + 0x38c) = 1;
    func_ov062_02117994(c, 4, 1);
}
