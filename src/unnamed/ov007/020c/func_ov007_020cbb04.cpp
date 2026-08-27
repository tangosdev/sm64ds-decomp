//cpp
struct Player { int St_EndingFly_Main(); };
extern "C" void func_ov007_020c8098(void* p);
extern "C" void func_ov007_020c7368(void* s);

extern "C" void func_ov007_020cbb04(char* c)
{
    ((Player*)*(void**)(c))->St_EndingFly_Main();
    ((Player*)*(void**)(c + 0x28))->St_EndingFly_Main();
    ((Player*)*(void**)(c + 0x2c))->St_EndingFly_Main();
    ((Player*)*(void**)(c + 0x30))->St_EndingFly_Main();
    ((Player*)*(void**)(c + 0x34))->St_EndingFly_Main();

    int i = 0;
    int* a = *(int**)(c + 4);
    if ((a[1] - 1) * (a[0] - 1) > 0) {
        do {
            func_ov007_020c8098(((void**)*(void**)(c + 8))[i]);
            i++;
            a = *(int**)(c + 4);
        } while (i < (a[1] - 1) * (a[0] - 1));
    }
    ((Player*)*(void**)(c + 8))->St_EndingFly_Main();
    func_ov007_020c7368(*(void**)(c + 4));
    ((Player*)c)->St_EndingFly_Main();
}
