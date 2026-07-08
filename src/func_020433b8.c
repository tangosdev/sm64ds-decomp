typedef unsigned char u8;

extern int PROCESS_STATE[];
extern int data_020a4b88[];
extern void func_0204335c(void* self);
extern int func_0203b244(void* l, void* n);

void func_020433b8(char* self)
{
    func_0204335c(self);
    if (*(u8*)(self + 0xf) != 0) return;
    if (*(u8*)(self + 0x10) != 0) return;
    {
        volatile u8* q = (u8*)(self + 0xe);
        int b = q[0] == 0 ? 1 : 0;
        if (b == 0) return;
    }
    {
        volatile int* p = PROCESS_STATE;
        int c = p[0] == 2 ? 1 : 0;
        if (c) {
            *(u8*)(self + 0x11) = 1;
            return;
        }
    }
    func_0203b244(data_020a4b88, self + 0x28);
}
