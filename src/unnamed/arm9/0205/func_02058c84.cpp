//cpp
extern "C" {

// @symbol func_02058d6c
void func_02058d6c(int idx, unsigned int val) {
    ((unsigned int *)0x27ffdc4)[idx] = val;
}

} // extern "C"

//cpp
extern "C" {

// @symbol func_02058d58
void func_02058d58(int idx, unsigned int val) {
    ((unsigned int *)0x27ffda0)[idx] = val;
}

} // extern "C"

//cpp
extern "C" {

extern unsigned int func_02058ea0(int idx);
extern unsigned int func_02058eb4(int idx);
extern void func_02058d58(int idx, unsigned int val);

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

//cpp
extern "C" {

extern void func_02058f28(void);
extern void func_0205b858(void);
extern void func_02057320(void);
extern void func_02058ec8(void);
extern void func_02057000(void);
extern void func_02059594(void);
extern void func_02059f48(int v);
extern void func_02059cb4(void);
extern void func_02058308(void);
extern void func_02059e48(void);
extern void func_0206a88c(void);
extern void func_02060890(void);
extern void func_0205fde8(void);
// @symbol func_02058c84
void func_02058c84(void)
{
    func_02058f28();
    func_0205b858();
    func_02057320();
    func_02058ec8();
    func_02057000();
    func_02059594();
    func_02059f48(3);
    func_02059cb4();
    func_02058308();
    func_02059e48();
    func_0206a88c();
    func_02060890();
    func_0205fde8();
}

} // extern "C"
