//cpp
extern "C" {
extern int _ZN5dCc_c5ClearEv(void*);
extern int _ZN10dBgCh_Actr15ClearGroundFlagEv(void*);
int func_ov030_021132d4(char* c) {
    int* p = (int*)((int)c + 0xb0);
    int tmp = *p;
    *p = tmp & ~0x80000;
    *(unsigned char*)(c + 0x3c7) = 0;
    void* arg1 = (void*)(c + 0x160);
    *(int*)(c + 0x98) = 0;
    _ZN5dCc_c5ClearEv(arg1);
    _ZN10dBgCh_Actr15ClearGroundFlagEv((void*)(c + 0x194));
    *(int*)(c + 0x3b8) = *(int*)(c + 0x3b4);
    *(int*)(c + 0x3b4) = 5;
    return 1;
}
}
