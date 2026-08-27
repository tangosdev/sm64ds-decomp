extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int saved);
extern void OS_WakeupThread(char* p);
extern void func_02058048(char* p);
typedef void (*CB)(void*);
void func_0206081c(char* obj) {
    CB cb = *(CB*)(obj + 0x28);
    void* arg = *(void**)(obj + 0x2c);
    unsigned int saved = _ZN3IRQ7DisableEv();
    *(int *)(((int)obj + 0x34)) &= ~0xc;
    OS_WakeupThread(obj + 0xd4);
    if (*(int*)(obj + 0x34) & 0x10) {
        func_02058048(obj + 0x3c);
    }
    _ZN3IRQ7RestoreEj(saved);
    if (cb != 0) {
        cb(arg);
    }
}
