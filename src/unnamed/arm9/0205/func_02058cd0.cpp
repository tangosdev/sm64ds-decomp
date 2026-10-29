//cpp

extern "C" {

extern unsigned int func_02058ea0(int idx);
extern unsigned int func_02058eb4(int idx);

// @symbol func_02058d6c
void func_02058d6c(int idx, unsigned int val) {
    ((unsigned int *)0x27ffdc4)[idx] = val;
}

// @symbol func_02058d58
void func_02058d58(int idx, unsigned int val) {
    ((unsigned int *)0x27ffda0)[idx] = val;
}

// @symbol func_02058cd0
unsigned int func_02058cd0(int idx, unsigned int size, unsigned int align) {
    unsigned int base = func_02058ea0(idx);
    unsigned int start, t, end;
    if (base == 0)
        return 0;
    start = (base + align - 1) & ~(align - 1);
    t = start + size;
    end = (t + align - 1) & ~(align - 1);
    if (end > func_02058eb4(idx))
        return 0;
    func_02058d58(idx, end);
    return start;
}

} // extern "C"
