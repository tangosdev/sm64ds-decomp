//cpp
extern "C" {
extern int _ZN6Player12CanEnterDoorEh(void*, unsigned char);
extern void func_020731dc(void* a, void* b, void* node);
extern int func_ov100_02145170(void* r0, void* r1, void* a, void* b);
extern int func_ov100_021453d8(void* c, void* p, int a2);
extern void SetTouchScreenDelay(void);
extern int data_ov100_021488f4[];
extern unsigned int data_ov100_0214870c;
extern int data_ov100_021487e4[];
extern unsigned int data_ov100_02148714;
extern int data_ov100_02148808[];
extern void func_020072c0(void);
extern int data_ov100_021487cc[];
extern int data_ov100_021487fc[];

int func_ov100_021451c4(char* r6, void* r5, char* r4) {
    if (r5 == 0) {
        r5 = data_ov100_021488f4;
    } else if (_ZN6Player12CanEnterDoorEh(r4, *(int*)(r6 + 0x88) <= 0) == 0) {
        return 0;
    }
    if (!(data_ov100_0214870c & 1)) {
        data_ov100_021487e4[0] = 0;
        data_ov100_021487e4[1] = 0;
        data_ov100_021487e4[2] = 0x64000;
        func_020731dc(data_ov100_021487e4, (void*)func_020072c0, data_ov100_021487cc);
        data_ov100_0214870c |= 1;
    }
    if (!(data_ov100_02148714 & 1)) {
        data_ov100_02148808[0] = 0;
        data_ov100_02148808[1] = 0;
        data_ov100_02148808[2] = -0x64000;
        func_020731dc(data_ov100_02148808, (void*)func_020072c0, data_ov100_021487fc);
        data_ov100_02148714 |= 1;
    }
    func_ov100_02145170(r6, r4, data_ov100_021487e4, data_ov100_02148808);
    func_ov100_021453d8(r6, r5, (int)r4);
    SetTouchScreenDelay();
    return 1;
}
}
