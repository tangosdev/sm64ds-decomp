extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int saved);
extern void func_02058988(void* list, char* node);
extern void func_0205807c(char* node);
struct G { char pad8[8]; int field8; };
extern struct G data_020a6134;
void func_02058a94(char* obj) {
    unsigned int saved = _ZN3IRQ7DisableEv();
    int g = data_020a6134.field8;
    if (*(int*)(obj + 4) == g) {
        int* cnt = (int*)(((int)(obj + 8)) & 0xFFFFFFFFFFFFFFFFULL);
        *cnt = *cnt - 1;
        if (*(int*)(obj + 8) == 0) {
            func_02058988((void*)g, obj);
            *(int*)(obj + 4) = 0;
            func_0205807c(obj);
        }
    }
    _ZN3IRQ7RestoreEj(saved);
}
