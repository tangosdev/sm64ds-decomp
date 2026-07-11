typedef unsigned int u32;
extern char data_020a8180[];
extern int data_020a6134[];
void func_020580f0(void* p);
void func_02060228(void* fn);
int func_0206062c(void* g);
extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int flags);

#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off))) & 0xFFFFFFFFFFFFFFFFLL))

int func_02060484(int a, int b, int c, int d, int e, int f)
{
    char* g = (char*)AT(data_020a8180, 0);
    unsigned int irq = _ZN3IRQ7DisableEv();
    if (*(u32*)(g + 0x34) & 4) {
        do {
            func_020580f0(g + 0xd4);
        } while (*(u32*)(g + 0x34) & 4);
    }
    *(u32*)AT(g, 0x34) |= 4;
    _ZN3IRQ7RestoreEj(irq);
    *(int*)(g + 0x18) = b;
    *(int*)(g + 0x1c) = a;
    *(int*)(g + 0x20) = c;
    *(int*)(g + 0x28) = d;
    *(int*)(g + 0x2c) = e;
    if (f != 0) {
        func_02060228((void*)func_0206062c);
        return 1;
    }
    *(int*)(data_020a8180 + 0xd0) = data_020a6134[2];
    func_0206062c(g);
    return (*(int**)g)[0] == 0;
}
