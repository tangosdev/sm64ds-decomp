extern void func_020393a4(int* p, int v);
extern void Platform_UpdateModelPosAndRotY(void* c);
extern int Platform_IsClsnInRange(void* c, int a, int b);
extern void Platform_UpdateClsnPosAndRot(void* c);
int _ZN19RotatingPlatformLll8BehaviorEv(char* c) {
    func_020393a4((int*)(c+0x124), 0x150000);
    if (*(unsigned char*)(c+0x324)) {
        int* py = (int*)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFF);
        *py = *py - 0x2000;
        int lim = *(int*)(c+0x320) - 0xc8000;
        if (*(int*)(c+0x60) < lim) *(int*)(c+0x60) = lim;
        *(unsigned char*)(c+0x324) = 0;
    } else {
        int* py = (int*)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFF);
        *py = *py + 0x2000;
        int lim = *(int*)(c+0x320);
        if (*(int*)(c+0x60) > lim) *(int*)(c+0x60) = lim;
    }
    Platform_UpdateModelPosAndRotY(c);
    if (Platform_IsClsnInRange(c, 0, 0)) {
        Platform_UpdateClsnPosAndRot(c);
    }
    return 1;
}
