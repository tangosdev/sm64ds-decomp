//cpp
extern "C" {
int _ZN6Player15IsCollectingCapEv(void* p);
extern unsigned char data_0208a0e0[];
extern void* data_0209f394[];

int func_ov001_020aa7b8(void* self, char* a)
{
    if (a == 0) return 0;
    if (*(unsigned char*)(a+0x19) == 3) return 1;
    int i = 0;
    for (i = 0; i < data_0208a0e0[0]; i++) {
        void* p = data_0209f394[i];
        if (p == 0) continue;
        if (_ZN6Player15IsCollectingCapEv(p)) return 0;
        if (self == *(void**)((char*)p+8)) return 0;
    }
    return 1;
}
}
