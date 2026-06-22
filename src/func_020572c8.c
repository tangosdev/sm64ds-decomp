typedef struct LockObj LockObj;
extern int func_02057198(unsigned val, LockObj *addr, void (*cleanupFn)(void), unsigned useAll);
extern void func_020570a8(void);

int func_020572c8(unsigned val, LockObj *addr, void (*cleanupFn)(void), unsigned useAll) {
    int r = func_02057198(val, addr, cleanupFn, useAll);
    if (r <= 0) return r;
    do {
        func_020570a8();
        r = func_02057198(val, addr, cleanupFn, useAll);
    } while (r > 0);
    return r;
}
